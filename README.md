# ICP and TrICP

Implementation of Iterative Closest Point and Trimmed Iterative Closest Point algorithms.

Initial, 10° rotation around the z axis to displace one of the LionScan pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/LionScan_rotated.PNG?raw=true)

After using ICP to register the 2 LionScan pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_LionScan_rotated.PNG?raw=true)

After using TrICP to register the 2 LionScan pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_LionScan_rotated.PNG?raw=true)

|Name | Angular rotation error | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP         |  9.999921165181412°   | 0.006538385757928085    | 5.14622    |       33.2789 s        |
| TrICP         |  9.999922591006124°     | 0.0005138325202816723       | 2.24015      |          10.1002 s     |


Initial, 10° rotation around the z axis to displace one of the fountain pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/fountain_rotated.PNG?raw=true)

After using ICP to register the 2 fountain pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_fountain_rotated.PNG?raw=true)

After using TrICP to register the 2 LionScan pointclouds.

![alt text](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_fountain_rotated.PNG?raw=true)

|Name | Angular rotation error | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP         |  10.004743176473797°   |0.006538385757928085    | 0.270625    |       24.8517 s       |
| TrICP         | 10.00113297679952°     | 0.0005138325202816723      | 0.00709813      |          20.028 s     |

TrICP is more robust to rotations and to incomplete, noisy data. 
TrICP runs fast than ICP and results in a lower MSE and Translation MSE values.
