# ICP and TrICP

Implementation of Iterative Closest Point and Trimmed Iterative Closest Point algorithms.

Initial, 10° rotation around the z axis to displace one of the LionScan pointclouds.            | Initial, 20° rotation around the z axis to displace one of the LionScan pointclouds.
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/LionScan_rotated.PNG?raw=true)  |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/LionScan_rotated_20.PNG?raw=true)

After using ICP to register the 2 LionScan pointclouds. (10°)            | After using TrICP to register the 2 LionScan pointclouds. (10°)
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_LionScan_rotated.PNG?raw=true)  |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_LionScan_rotated.PNG?raw=true)

After using ICP to register the 2 LionScan pointclouds. (20°)            | After using TrICP to register the 2 LionScan pointclouds. (20°)
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_LionScan_rotated_20.PNG?raw=true)  |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_LionScan_rotated_20.PNG?raw=true)

|Name | Angular rotation difference | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP (10°)         |  9.999921165181412°   | 0.006538385757928085    | 5.14622    |       33.2789 s        |
| TrICP (10°)       |  9.999922591006124°     | 0.0005138325202816723       | 2.24015      |          10.1002 s     |
| ICP (20°)         |  19.99993772618883°   | 0.0000003388619386119256    | 5.14622   |       34.7251 s        |
| TrICP (20°)       |  19.999940241038647°     | 0.000001488607811792912       | 4.55636      |          13.7501 s     |


Initial, 10° rotation around the z axis to displace one of the fountain pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/fountain_rotated.PNG?raw=true)

After using ICP to register the 2 fountain pointclouds. (10°)

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_fountain_rotated.PNG?raw=true)

After using TrICP to register the 2 LionScan pointclouds. (10°)

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_fountain_rotated.PNG?raw=true)

Initial, 20° rotation around the z axis to displace one of the fountain pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/fountain_rotated_20.PNG?raw=true)

After using ICP to register the 2 fountain pointclouds. (20°)

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_fountain_rotated_20.PNG?raw=true)

After using TrICP to register the 2 LionScan pointclouds. (20°)

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_fountain_rotated_20.PNG?raw=true)

|Name | Angular rotation error | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP     (10°)    |  10.004743176473797°   |0.006538385757928085    | 0.270625    |       24.8517 s       |
| TrICP    (10°)      | 10.00113297679952°     | 0.0005138325202816723      | 0.00709813      |          20.028 s     |
| ICP     (20°)     |  19.99996534374046°   | 0.000020926850126866322    | 0.251174    |       63.4932 s       |
| TrICP     (20°)     | 19.999991193277836°     | 0.00012271255912062388      | 0.0238604     |         13.1965 s     |

TrICP is more robust to rotations and to incomplete, noisy data. 
TrICP runs fast than ICP and results in a lower MSE and Translation MSE values.
