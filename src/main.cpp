#include "../nanoflann.hpp"
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Eigen/Dense>
#include "MatrixReaderWriter.h"
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <chrono>

using namespace Eigen;
using namespace std;
using namespace nanoflann;
using namespace cv;
typedef KDTreeEigenMatrixAdaptor<MatrixXd> my_kd_tree_t;
vector<int> indices_;
vector<double> dists_;
vector<tuple<int, int, double>> pairedIndicesAndDistances;
typedef struct Trafo {
    Mat rot;
    Mat transl;
} Trafo;
#define PI 3.14159265

template <typename Der>
void generateRandomPointCloud(MatrixBase<Der> &mat, const size_t N, const size_t dim, const typename Der::Scalar max_range = 10) {
  std::cout << "Generating " << N << " random points...";
  mat.resize(N, dim);
  for (size_t i = 0; i < N; i++)
    for (size_t d = 0; d < dim; d++)
      mat(i, d) = max_range * (rand() % 1000) / typename Der::Scalar(1000);

  std::cout << "done\n";
}

void loadPointCloud(MatrixXd &mat1, MatrixXd &mat2, MatrixReaderWriter& mrw1, MatrixReaderWriter& mrw2) {
    mat1.resize(mrw1.rowNum, mrw1.columnNum);
    #pragma omp parallel for
    for (size_t i = 0; i < mrw1.rowNum; i++)
        for (size_t d = 0; d < mrw1.columnNum; d++)
            mat1(i, d) = mrw1.data[mrw1.columnNum * i + d];

    cout << "Point cloud 1 loaded\n";

    mat2.resize(mrw2.rowNum, mrw2.columnNum);
    #pragma omp parallel for
    for (size_t i = 0; i < mrw2.rowNum; i++)
        for (size_t d = 0; d < mrw2.columnNum; d++)
            mat2(i, d) = mrw2.data[mrw2.columnNum * i + d];

    cout << "Point cloud 2 loaded\n";
}

void findNearestNeighbours(Mat mat1, const my_kd_tree_t& mat_index, const size_t rowNum, const size_t columnNum) {
  indices_.clear();
  dists_.clear();
  for (int i = 0; i < rowNum; ++i) {
    // Query point:
    vector<double> query_pt(columnNum);
    for (size_t d = 0; d < columnNum; d++) {
        query_pt[d] = mat1.at<double>(i, d);
    }
    // do a knn search
    size_t ret_index;
    double out_dist_sqr;

    nanoflann::KNNResultSet<double> resultSet(1);
    resultSet.init(&ret_index, &out_dist_sqr);

    mat_index.index->findNeighbors(resultSet, &query_pt[0],nanoflann::SearchParams(10));

    indices_.push_back(int(ret_index));
    dists_.push_back(out_dist_sqr);
  }
}

Trafo best_fit_transform(const Mat& pointcloud_1, const Mat& pointcloud_2) {
    Point3d center1, center2;
    Mat R, T;
    R = Mat::zeros(3, 3, CV_64F);
    T = Mat::zeros(3, 1, CV_64F);

    Mat covariance = Mat::zeros(3, 3, CV_64F);
    vector<Point3d> cent_pcl1, cent_pcl2;
    Trafo returnT;

    int pcl1_rows = pointcloud_1.rows, pcl2_rows = pointcloud_2.rows;
    // Calculate the center of pcl1
    for (int i = 0; i < pcl1_rows; i++) {
        center1.x += pointcloud_1.at<double>(i, 0);
        center1.y += pointcloud_1.at<double>(i, 1);
        center1.z += pointcloud_1.at<double>(i, 2);
    }
    center1 = center1 * (1.0 / pcl1_rows);

    // Calculate the center of pcl2
    for (int i = 0; i < pcl2_rows; i++) {
        center2.x += pointcloud_2.at<double>(i, 0);
        center2.y += pointcloud_2.at<double>(i, 1);
        center2.z += pointcloud_2.at<double>(i, 2);
    }
    center2 = center2 * (1.0 / pcl2_rows);

    // Move pcl1 to the center
    for (int i = 0; i < pcl1_rows; i++) {
        Point3d pt;
        pt.x = pointcloud_1.at<double>(i, 0) - center1.x;
        pt.y = pointcloud_1.at<double>(i, 1) - center1.y;
        pt.z = pointcloud_1.at<double>(i, 2) - center1.z;
        cent_pcl1.emplace_back(pt);
    }

    // Move pcl2 to the center
    for (int i = 0; i < pcl2_rows; i++) {
        Point3d pt;
        pt.x = pointcloud_2.at<double>(i, 0) - center2.x;
        pt.y = pointcloud_2.at<double>(i, 1) - center2.y;
        pt.z = pointcloud_2.at<double>(i, 2) - center2.z;
        cent_pcl2.emplace_back(pt);
    }

    // Calculate covariance matrix
    for (int i = 0; i < pcl1_rows; i++) {
        covariance.at<double>(0, 0) += cent_pcl1[i].x * cent_pcl2[i].x;
        covariance.at<double>(0, 1) += cent_pcl1[i].x * cent_pcl2[i].y;
        covariance.at<double>(0, 2) += cent_pcl1[i].x * cent_pcl2[i].z;
        covariance.at<double>(1, 0) += cent_pcl1[i].y * cent_pcl2[i].x;
        covariance.at<double>(1, 1) += cent_pcl1[i].y * cent_pcl2[i].y;
        covariance.at<double>(1, 2) += cent_pcl1[i].y * cent_pcl2[i].z;
        covariance.at<double>(2, 0) += cent_pcl1[i].z * cent_pcl2[i].x;
        covariance.at<double>(2, 1) += cent_pcl1[i].z * cent_pcl2[i].y;
        covariance.at<double>(2, 2) += cent_pcl1[i].z * cent_pcl2[i].z;
    }
    covariance /= pcl1_rows;
    Mat w, u, vt;
    SVD::compute(covariance, w, u, vt, 0);

    // Calculate the rotation matrix
    R = vt.t() * u.t();
    if (determinant(R) < 0.) {
        vt.at<double>(2, 0) *= -1;
        vt.at<double>(2, 1) *= -1;
        vt.at<double>(2, 2) *= -1;
        R = vt.t() * u.t();
    }

    Mat centerOfMass1 = Mat::zeros(3, 1, CV_64F); 
    centerOfMass1.at<double>(0, 0) = center1.x;
    centerOfMass1.at<double>(1, 0) = center1.y;
    centerOfMass1.at<double>(2, 0) = center1.z;

    Mat centerOfMass2 = Mat::zeros(3, 1, CV_64F);
    centerOfMass2.at<double>(0, 0) = center2.x;
    centerOfMass2.at<double>(1, 0) = center2.y;
    centerOfMass2.at<double>(2, 0) = center2.z;

    // Calculate the translation matrix
    T = centerOfMass2 - (R * centerOfMass1);

    returnT.rot = R;
    returnT.transl = T;
    return returnT;
}

bool sortbythird(const tuple<int, int, double>& a, const tuple<int, int, double>& b){
    return (get<2>(a) < get<2>(b));
}

//Matrix3d eulerAnglesToRotationMatrix(vector<double>& theta)
//{
//    Matrix3d R_x = Matrix3d::Identity(3, 3);
//    // Calculate rotation about x axis
//    R_x <<
//        1, 0, 0,
//        0, cos(theta[0]), -sin(theta[0]),
//        0, sin(theta[0]), cos(theta[0]);
//
//    // Calculate rotation about y axis
//    Matrix3d R_y = Matrix3d::Identity(3, 3);
//    R_y <<
//        cos(theta[1]), 0, sin(theta[1]),
//        0, 1, 0,
//        -sin(theta[1]), 0, cos(theta[1]);
//
//    // Calculate rotation about z axis
//    Matrix3d R_z = Matrix3d::Identity(3, 3);
//    R_z <<
//        cos(theta[2]), -sin(theta[2]), 0,
//        sin(theta[2]), cos(theta[2]), 0,
//        0, 0, 1;
//
//    // Combined rotation matrix
//    return R_z * R_y * R_x;
//}

double calculateError(Mat mat1, Mat mat2, Mat rot, Mat transl) {
    double error = 0;
    for (int i = 0; i < mat2.rows ; i++) {
        Mat pont1 = Mat::zeros(3, 1, CV_64F);
        Mat pont2 = Mat::zeros(3, 1, CV_64F);
        pont1.at<double>(0, 0) = mat1.at<double>(i, 0);
        pont1.at<double>(1, 0) = mat1.at<double>(i, 1);
        pont1.at<double>(2, 0) = mat1.at<double>(i, 2);
        pont2.at<double>(0, 0) = mat2.at<double>(i, 0);
        pont2.at<double>(1, 0) = mat2.at<double>(i, 1);
        pont2.at<double>(2, 0) = mat2.at<double>(i, 2);
        Mat diff = (rot * pont1 + transl) - pont2;
        error += sqrt(diff.at<double>(0, 0) * diff.at<double>(0, 0) + diff.at<double>(1, 0) * diff.at<double>(1, 0) + diff.at<double>(2, 0) * diff.at<double>(2, 0));
    }
    error = error / mat2.rows;
    return error;
}

void icp(MatrixXd mat1, MatrixXd mat2, int max_iteration_num) {
    auto time_start = std::chrono::high_resolution_clock::now();
    Trafo T;
    Mat cvSource, cvTarget;
    eigen2cv(mat1, cvSource);
    eigen2cv(mat2, cvTarget);
    double prev_error = 0.0;
    double mean_error = 0.0;
    double tolerance = 0.00001;
    int max_iters_ = max_iteration_num;//100;
    my_kd_tree_t mat_index(3, std::cref(mat2), 10 /* max leaf */);
    mat_index.index->buildIndex();

    for (int iters_ = 0; iters_ < max_iters_; iters_++) {
        findNearestNeighbours(cvSource, mat_index, cvSource.rows, 3);
        Mat cvNewTarget = Mat::zeros(cvSource.rows, 3, CV_64F);
        for (int i = 0; i < cvSource.rows; i++) {
            cvNewTarget.at<double>(i, 0) = cvTarget.at<double>(indices_[i], 0);
            cvNewTarget.at<double>(i, 1) = cvTarget.at<double>(indices_[i], 1);
            cvNewTarget.at<double>(i, 2) = cvTarget.at<double>(indices_[i], 2);
        }
        T = best_fit_transform(cvSource, cvNewTarget); // Compute motion that minimises mean square error(MSE) between paired points.

        for (int i = 0; i < cvSource.rows; i++) {     // Apply motion to P and update MSE.
            Mat pont = Mat::zeros(3, 1, CV_64F);
            pont.at<double>(0, 0) = cvSource.at<double>(i, 0);
            pont.at<double>(1, 0) = cvSource.at<double>(i, 1);
            pont.at<double>(2, 0) = cvSource.at<double>(i, 2);
            pont = T.rot * pont;
            pont += T.transl;
            cvSource.at<double>(i, 0) = pont.at<double>(0, 0);
            cvSource.at<double>(i, 1) = pont.at<double>(1, 0);
            cvSource.at<double>(i, 2) = pont.at<double>(2, 0);
        }

        mean_error = calculateError(cvSource, cvNewTarget, T.rot, T.transl); // Updating MSE
        cout << mean_error << endl;
        if (abs(prev_error - mean_error) < tolerance) {
            break;
        }
        prev_error = mean_error;
    }
    auto time_finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (time_finish - time_start);
    cout << "Time = " << duration.count() << " s" << endl;
    ofstream file("D:/source/repos/3dsens_icp/test.xyz");
    if (file.is_open())
    {
        for (int i = 0; i < cvSource.rows; ++i) {
            file << cvSource.at<double>(i, 0) << " " << cvSource.at<double>(i, 1) << " " << cvSource.at<double>(i, 2) << endl;
        }
        for (int i = 0; i < cvTarget.rows; ++i) {
            file << cvTarget.at<double>(i, 0) << " " << cvTarget.at<double>(i, 1) << " " << cvTarget.at<double>(i, 2) << endl;
        }
    }
}

void tricp(MatrixXd mat1, MatrixXd mat2, int max_iteration_num) {
    auto time_start = std::chrono::high_resolution_clock::now();
    Trafo T;
    Mat cvSource, cvTarget;
    eigen2cv(mat1, cvSource);
    eigen2cv(mat2, cvTarget);
    double prev_error = 0.0;
    double mean_error = 0.0;
    double tolerance = 0.00001;
    int max_iters_ = max_iteration_num;//100;
    my_kd_tree_t mat_index(3, std::cref(mat2), 10 /* max leaf */);
    mat_index.index->buildIndex();

    int NPo = 1000;
    for (int iters_ = 0; iters_ < max_iters_; iters_++) {
        pairedIndicesAndDistances.clear();
        findNearestNeighbours(cvSource, mat_index, cvSource.rows, 3); 

        for (int i = 0; i < cvSource.rows; i++) {
            pairedIndicesAndDistances.push_back(make_tuple(i, indices_[i], dists_[i]));
        }

        sort(pairedIndicesAndDistances.begin(), pairedIndicesAndDistances.end(), sortbythird);
        sort(dists_.begin(), dists_.end());

        Mat cvNewSource = Mat::zeros(NPo, 3, CV_64F);
        Mat cvNewTarget = Mat::zeros(NPo, 3, CV_64F);
        for (int i = 0; i < NPo; i++) {
            cvNewSource.at<double>(i, 0) = cvTarget.at<double>(get<0>(pairedIndicesAndDistances[i]), 0);
            cvNewSource.at<double>(i, 1) = cvTarget.at<double>(get<0>(pairedIndicesAndDistances[i]), 1);
            cvNewSource.at<double>(i, 2) = cvTarget.at<double>(get<0>(pairedIndicesAndDistances[i]), 2);
            cvNewTarget.at<double>(i, 0) = cvTarget.at<double>(get<1>(pairedIndicesAndDistances[i]), 0);
            cvNewTarget.at<double>(i, 1) = cvTarget.at<double>(get<1>(pairedIndicesAndDistances[i]), 1);
            cvNewTarget.at<double>(i, 2) = cvTarget.at<double>(get<1>(pairedIndicesAndDistances[i]), 2);
        }

        T = best_fit_transform(cvNewSource, cvNewTarget); // For !!!! Npo selected pairs !!!!, compute optimal motion(R, t) that minimises STS

        for (int i = 0; i < cvSource.rows; i++) {     // Apply motion to P and update MSE.
            Mat pont = Mat::zeros(3, 1, CV_64F);
            pont.at<double>(0, 0) = cvSource.at<double>(i, 0);
            pont.at<double>(1, 0) = cvSource.at<double>(i, 1);
            pont.at<double>(2, 0) = cvSource.at<double>(i, 2);
            pont = T.rot * pont;
            pont += T.transl;
            cvSource.at<double>(i, 0) = pont.at<double>(0, 0);
            cvSource.at<double>(i, 1) = pont.at<double>(1, 0);
            cvSource.at<double>(i, 2) = pont.at<double>(2, 0);
        }
        mean_error = calculateError(cvSource, cvNewTarget, T.rot, T.transl); // Updating MSE
        cout << mean_error << endl;

        if (abs(prev_error - mean_error) < tolerance) {
            break;
        }
        prev_error = mean_error;
    }
    auto time_finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (time_finish - time_start);
    cout << "Time = " << duration.count() << " s" << endl;
    ofstream file("D:/source/repos/3dsens_icp/test_trimmed.xyz");
    if (file.is_open())
    {
        for (int i = 0; i < cvSource.rows; ++i) {
            file << cvSource.at<double>(i, 0) << " " << cvSource.at<double>(i, 1) << " " << cvSource.at<double>(i, 2) << endl;
        }
        for (int i = 0; i < cvTarget.rows; ++i) {
            file << cvTarget.at<double>(i, 0) << " " << cvTarget.at<double>(i, 1) << " " << cvTarget.at<double>(i, 2) << endl;
        }
    }
}

int main(int argc, char **argv) {
  // Randomize Seed
  srand(static_cast<unsigned int>(time(nullptr)));
  if (argc < 3) {
      std::cerr << "Usage: " << argv[0] << " XYZ1 XYZ2 MAX_ITER" << std::endl;
      return 1;
  }
  MatrixReaderWriter* mrw1;
  mrw1 = new MatrixReaderWriter(argv[1]); //"D:/source/repos/icp_trcip/sphere1.xyz");
  MatrixReaderWriter* mrw2;
  mrw2 = new MatrixReaderWriter(argv[2]); //"D:/source/repos/icp_trcip/sphere2.xyz");
  MatrixXd mat1(mrw1->rowNum, mrw1->columnNum);
  MatrixXd mat2(mrw2->rowNum, mrw2->columnNum);
  loadPointCloud(mat1, mat2, *mrw1, *mrw2);

  int max_iteration_num = atoi(argv[3]);

  // Iterative Closest Point Algorithm
  // icp(mat1, mat2, max_iteration_num);

  // Trimmed Iterative Closest Point Algorithm
  tricp(mat1, mat2, max_iteration_num); // mat1 is data, mat2 is the model

  return 0;
}