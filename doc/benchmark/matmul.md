## 矩阵乘法

平台 ubuntu 22.04
cpu  Intel(R) Core(TM) i9-14900K
内存 64GB

| 矩阵大小     | 耗时(秒) | 内存占用   |
|--------------|-----------|------------|
| 64x64        | 0.000073  | 16 KB      |
| 128x128      | 0.007146  | 64 KB      |
| 256x256      | 0.002196  | 256 KB     |
| 512x512      | 0.007013  | 1 MB       |
| 1024x1024    | 0.027820  | 4 MB       |
| 2048x2048    | 0.058486  | 16 MB      |
| 4096x4096    | 0.249994  | 64 MB      |
| 8192x8192    | 1.973990  | 256 MB     |
| 16384x16384  | 14.712000 | 1 GB       |
| 32768x32768  | 111.222000| 4 GB       |

再大会段错误，待优化
