#include <iomanip>
#include <sstream>
#include <ctime>

#include "deepx/tf/tf.hpp"
#include "stdutil/time.hpp"
namespace deepx::tf
{
    std::pair<string, string> split_body_metadata(const string &input)
    {
        size_t meta_pos = input.find("//");
        string body = input.substr(0, meta_pos);
        string meta = (meta_pos != string::npos) ? input.substr(meta_pos + 2) : "";
        return {body, meta};
    }

    std::tuple<string, string, string> split_func_input_output(const string &body)
    {
        size_t arrow_pos = body.find("->");
        if (arrow_pos == string::npos)
        {
            throw runtime_error("Invalid IR format: missing arrow");
        }

        // 获取输入和输出部分的原始字符串
        string input_part = body.substr(0, arrow_pos);
        string output_part = body.substr(arrow_pos + 2);

        // 提取函数名
        size_t space_pos = input_part.find(' ');
        size_t paren_pos = input_part.find('(');
        size_t name_end = std::min(
            space_pos != string::npos ? space_pos : input_part.length(),
            paren_pos != string::npos ? paren_pos : input_part.length());
        string func_name = input_part.substr(0, name_end);

        // 处理输入部分，去掉函数名
        input_part = input_part.substr(name_end);

        // 处理输入部分的括号
        size_t input_paren_start = input_part.find('(');
        size_t input_paren_end = input_part.rfind(')');
        if (input_paren_start != string::npos && input_paren_end != string::npos)
        {
            // 如果有括号，去掉括号
            input_part = input_part.substr(input_paren_start + 1, input_paren_end - input_paren_start - 1);
        }
        else
        {
            // 如果没有括号，去掉可能的前导空格
            if (!input_part.empty() && input_part[0] == ' ')
            {
                input_part = input_part.substr(1);
            }
        }

        // 处理输出部分的括号
        size_t output_paren_start = output_part.find('(');
        size_t output_paren_end = output_part.rfind(')');
        if (output_paren_start != string::npos && output_paren_end != string::npos)
        {
            // 如果有括号，去掉括号
            output_part = output_part.substr(output_paren_start + 1, output_paren_end - output_paren_start - 1);
        }

        return {func_name, input_part, output_part};
    }

    // 检查是否是数组格式并提取内容
    std::pair<bool, string> extract_array_content(const string &token) {
        if (token.front() != '[') {
            return {false, ""};
        }
        
        if (token.back() == ']') {
            // 单个token的数组
            return {true, token.substr(1, token.length() - 2)};
        }
        return {true, token.substr(1)}; // 返回去掉开头'['的内容
    }

    // 处理多token数组的情况
    string parse_multi_token_array(stringstream &ss, const string &first_content) {
        string array_content = first_content;
        string temp;
        
        while (ss >> temp) {
            array_content += " " + temp;
            if (temp.back() == ']') {
                return array_content.substr(0, array_content.length() - 1);
            }
        }
        throw runtime_error("Invalid array format: missing closing bracket");
    }

    // 解析单个值为具体C++类型
    any parse_single_value(const string &value_str, const TypeDef &dtype) {
        // 如果是字符串类型，直接返回
        if (dtype.precision() == Precision::String) {
            return value_str;
        }

        // 处理布尔类型
        if (dtype.precision() == Precision::Bool) {
            if (value_str == "true" || value_str == "1") return true;
            if (value_str == "false" || value_str == "0") return false;
            throw runtime_error("Invalid boolean value: " + value_str);
        }

        try {
            // 处理整数类型
            if (uint16_t(dtype.precision() & Precision::Int)) {
                if (dtype.precision() == Precision::Int64) return stoll(value_str);
                if (dtype.precision() == Precision::Int32) return stoi(value_str);
                if (dtype.precision() == Precision::Int16) return (int16_t)stoi(value_str);
                if (dtype.precision() == Precision::Int8) return (int8_t)stoi(value_str);
                if (dtype.precision() == Precision::Int4) return (int8_t)(stoi(value_str) & 0x0F);
            }
            
            // 处理浮点类型
            if (uint16_t(dtype.precision() & Precision::Float) ) {
                if (dtype.precision() == Precision::Float64) return stod(value_str);
                if (dtype.precision() == Precision::Float32) return stof(value_str);
                // Float16等其他浮点类型先转为float32
                return stof(value_str);
            }

            // Any类型，尝试按照以下顺序解析：int64 -> float64 -> bool -> string
            try {
                return stoll(value_str);
            } catch (...) {
                try {
                    return stod(value_str);
                } catch (...) {
                    if (value_str == "true") return true;
                    if (value_str == "false") return false;
                    return value_str;
                }
            }
        } catch (const std::exception &e) {
            throw runtime_error("Failed to parse value '" + value_str + "' as " + 
                              base_category_str(dtype.category()) + "<" + 
                              precision_str(dtype.precision()) + ">");
        }
        
        return value_str; // 默认作为字符串处理
    }

    // 解析数组值
    vector<any> parse_array_values(const string &array_str, const TypeDef &elem_dtype) {
        vector<any> values;
        stringstream ss(array_str);
        string elem_str;
        
        while (ss >> elem_str) {
            values.push_back(parse_single_value(elem_str, elem_dtype));
        }
        
        return values;
    }

    // // 修改parse_array_param函数
    // Param parse_array_param(stringstream &ss, const string &array_content, bool call) {
    //     if (call) {
    //         // callfunc模式：解析数组值
    //         Param param("[" + array_content + "]", "vector<any>");
    //         param.value = parse_array_values(array_content, 
    //             make_dtype(DataCategory::Unknown, Precision::Any));
    //         return param;
    //     } else {
    //         // deffunc模式：第二个token是参数名
    //         string param_name;
    //         ss >> param_name;
    //         TypeDef type = dtype(array_content);
    //         return Param(param_name, type);
    //     }
    // }

    // // 修改parse_non_array_param函数
    // Param parse_non_array_param(stringstream &ss, const string &first_token, bool call) {
    //     string second_token;
    //     if (ss >> second_token) {
    //         if (call) {
    //             // callfunc模式：first_token是类型，second_token是值
    //             Param param(second_token, first_token);
    //             param.value = parse_single_value(second_token, param.dtype);
    //             return param;
    //         } else {
    //             // deffunc模式：first_token是类型，second_token是参数名
    //             return Param(second_token, first_token);
    //         }
    //     } else {
    //         // 只有一个token
    //         if (call) {
    //             // callfunc模式：token作为值
    //             Param param(first_token, "any");
    //             param.value = parse_single_value(first_token, param.dtype);
    //             return param;
    //         } else {
    //             // deffunc模式：token作为类型
    //             return Param("", first_token);
    //         }
    //     }
    // }

    // 主参数解析函数
    Param parse_param(const string &param_str, bool call) {
        stringstream param_ss(param_str);
        string first_token;
        
        if (!(param_ss >> first_token)) {
            return Param();
        }
        
        // 检查是否是数组格式
        // auto [is_array, array_content] = extract_array_content(first_token);
        // if (is_array) {
        //     if (array_content.empty() || array_content.back() != ']') {
        //         // 处理多token数组
        //         array_content = parse_multi_token_array(param_ss, array_content);
        //     }
        //     return parse_array_param(param_ss, array_content, call);
        // }
        
        // // 处理非数组格式
        // return parse_non_array_param(param_ss, first_token, call);
        return Param{};
    }

    // 解析参数列表
    vector<Param> parse_params(const string &params_str, bool call)
    {
        vector<Param> params;
        stringstream params_ss(params_str);
        string param;

        while (getline(params_ss, param, ','))
        {
            // 去除首尾空格
            param.erase(0, param.find_first_not_of(" "));
            param.erase(param.find_last_not_of(" ") + 1);

            if (param.empty())
                continue;

            // 解析单个参数
            Param parsed_param = parse_param(param, call);
            params.push_back(parsed_param);
        }

        return params;
    }

    // 解析元数据键值对
    void parse_metadata_pair(const string &key_value, int &id, string &author,
                             system_clock::time_point &created_at,
                             system_clock::time_point &sent_at)
    {
        size_t eq_pos = key_value.find('=');
        if (eq_pos == string::npos)
            return;

        string key = key_value.substr(0, eq_pos);
        string value = key_value.substr(eq_pos + 1);

        if (key == "id")
        {
            id = stoi(value);
        }
        else if (key == "author")
        {
            author = value;
        }
        else if (key == "created_at")
        {
            created_at = system_clock::from_time_t(stod(value));
        }
        else if (key == "sent_at")
        {
            sent_at = system_clock::from_time_t(stod(value));
        }
    }

    // 解析元数据
    void parse_metadata(const string &meta, int &id, string &author,
                        system_clock::time_point &created_at,
                        system_clock::time_point &sent_at)
    {
        if (meta.empty())
            return;

        stringstream meta_ss(meta);
        string key_value;
        while (meta_ss >> key_value)
        {
            parse_metadata_pair(key_value, id, author, created_at, sent_at);
        }
    }

    // 主解析函数
    void TF::parse(const string &input, bool call)
    {
        // 1. 按//分割主体和元数据
        auto [body, meta] = split_body_metadata(input);

        // 2. 按->分割输入和输出，同时获取函数名
        auto [func_name, input_part, output_part] = split_func_input_output(body);
        name = func_name;

        // 3. 解析输入为参数列表
        args = parse_params(input_part, call);

        // 4. 解析返回值
        returns = parse_params(output_part, call);

        // 5. 解析元数据
        parse_metadata(meta, id, author, created_at, sent_at);
    }

    void TF::init(const string &opname,
                  const vector<Param> &args,
                  const vector<Param> &returns)
    {
        this->name = opname;
        this->author = "";
        this->args = args;
        this->returns = returns;
    }

    std::string TF::to_string(bool show_extra, bool show_name) const
    {
        std::stringstream ss;
        ss << name << "(";

        // 处理输入参数
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (i > 0)
            {
                ss << ", "; // 始终使用逗号分隔参数
            }

            // 输出类型，根据show_name决定是否输出参数名
            ss << dtype_str(args[i].dtype);
            if (show_name)
            {
                ss << " " << args[i].name;
            }
        }

        ss << ")->(";

        // 处理返回值
        for (size_t i = 0; i < returns.size(); ++i)
        {
            if (i > 0)
            {
                ss << ", "; // 始终使用逗号分隔返回值
            }

            // 输出类型，根据show_name决定是否输出返回值名
            ss << dtype_str(returns[i].dtype);
            if (show_name)
            {
                ss << " " << returns[i].name;
            }
        }

        ss << ")";

        if (show_extra)
        {
            ss << " //id=" << id
               << " created_at=" << stdutil::format_time(created_at)
               << " sent_at=" << stdutil::format_time(sent_at);
        }

        return ss.str();
    }

    TF::TF(string text, bool call)
    {
        parse(text, call);
    }
    string TF::op_name()
    {
        return name;
    }

    string TF::math_formula() const
    {
        return "";
    }

    std::string TF::dtypes() const
    {
        std::string full = to_string(false, false);
        size_t start = full.find('(');
        if (start != std::string::npos)
        {
            return full.substr(start);
        }
        return full;
    }

    bool TF::check_dtype(const TF &other) const
    {
        // 检查参数数量是否匹配
        if (args.size() != other.args.size() || returns.size() != other.returns.size())
        {
            return false;
        }

        // 检查每个参数的类型
        for (size_t i = 0; i < args.size(); ++i)
        {
            // 当前TF的类型可能包含多个选项
            TypeDef dtype = args[i].dtype;
            TypeDef other_dtype = other.args[i].dtype;
            //TODO
        }

        return true;
    }

    void TF::funcdef(int polymorphism) {
        // 基类的默认实现为空
        // 派生类需要重写这个函数来定义具体的函数签名
    }
}