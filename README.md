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

Angular rotation error = arccos((Tr(R'*Rgroundtruth) - 1) / 2)

|Name | Angular rotation error | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP (10°)         |  43.61895188360379°   | 42.12371448351258    | 5.14622    |       33.2789 s        |
| TrICP (10°)       |  12.074753762145663°     | 5.3375604492657045       | 2.24015      |          10.1002 s     |
| ICP (20°)         |  43.36017013541045°   | 38.286774289962146    | 5.14622   |       34.7251 s        |
| TrICP (20°)       |  18.911658794566836°     | 5.331268631078188       | 4.55636      |          13.7501 s     |

Initial, 10° rotation around the z axis to displace one of the fountain pointclouds.           | Initial, 20° rotation around the z axis to displace one of the fountain pointclouds.
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/fountain_rotated.PNG?raw=true)  |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/fountain_rotated_20.PNG?raw=true)

After using ICP to register the 2 fountain pointclouds. (10°)          | After using TrICP to register the 2 fountain pointclouds. (10°)
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_fountain_rotated.PNG?raw=true) |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_fountain_rotated.PNG?raw=true)

After using ICP to register the 2 fountain pointclouds. (20°)          | After using TrICP to register the 2 fountain pointclouds. (20°)
:-------------------------:|:-------------------------:
![](https://github.com/nyakasko/3dsens_icp/blob/main/images/icp_registered_fountain_rotated_20.PNG?raw=true) |  ![](https://github.com/nyakasko/3dsens_icp/blob/main/images/tricp_registered_fountain_rotated_20.PNG?raw=true)

Angular rotation error = arccos((Tr(R'*Rgroundtruth) - 1) / 2)

|Name | Angular rotation error | Translation MSE | MSE | Runtime |
| :---         | :---         |     :---:      |          ---: |          ---: |
| ICP     (10°)    |  6.282617302451196°   | 9.00814058823539   | 0.270625    |       24.8517 s       |
| TrICP    (10°)      | 11.218936278870313°     | 0.06764219165830683      | 0.00709813      |          13.1458 s     |
| ICP     (20°)     |  26.351965024001462°   | 59.101602791646876    | 0.251174    |       63.4932 s       |
| TrICP     (20°)     | 29.45099774151091°     | 38.8969257815729      | 0.0238604     |         13.1965 s     |

TrICP is more robust to rotations and to incomplete, noisy data. 
TrICP runs faster than ICP and results in lower MSE values.
