/**
* This file is part of ORB-SLAM3
*
* Copyright (C) 2017-2021 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-SLAM3 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM3 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-SLAM3.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <ctime>
#include <sstream>

#include<opencv2/core/core.hpp>

#include<System.h>
#include "ImuTypes.h"

using namespace std;

void LoadImagesTUMVI(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps);

void LoadIMU(const string &strImuPath, vector<double> &vTimeStamps, vector<cv::Point3f> &vAcc, vector<cv::Point3f> &vGyro);


double ttrack_tot = 0;
int main(int argc, char **argv)
{
    const int num_seq = 1;
    bool bFileName= false;

    string file_name;
    if (bFileName)
        file_name = string(argv[argc-1]);

    if(argc < 6)
    {
        cerr << endl << "Usage: ./mono_inertial_tum_vi path_to_vocabulary path_to_settings path_to_image_folder_1 path_to_times_file_1 path_to_imu_data_1 (path_to_image_folder_2 path_to_times_file_2 path_to_imu_data_2 ... path_to_image_folder_N path_to_times_file_N path_to_imu_data_N) (trajectory_file_name)" << endl;
        return 1;
    }

    // Load all sequences:
    int seq;
    vector<string> vstrImageFilenames;
    vector<double> vTimestampsCam;
    vector<cv::Point3f> vAcc, vGyro;
    vector<double> vTimestampsImu;
    int nImages = 0;
    int nImu = 0;
    int first_imu = 0;

    int tot_images = 0;
    cout << "Loading images for sequence " << seq << "...";
    LoadImagesTUMVI(string(argv[3*(seq+1)]), string(argv[3*(seq+1)+1]), vstrImageFilenames, vTimestampsCam);
    cout << "LOADED!" << endl;

    cout << "Loading IMU for sequence " << seq << "...";
    LoadIMU(string(argv[3*(seq+1)+2]), vTimestampsImu, vAcc, vGyro);
    cout << "LOADED!" << endl;

    nImages = vstrImageFilenames.size();
    tot_images += nImages;
    nImu = vTimestampsImu.size();

    if((nImages<=0)||(nImu<=0))
    {
      cerr << "ERROR: Failed to load images or IMU for sequence" << seq << endl;
      return 1;
    }

    // Find first imu to be considered, supposing imu measurements start first

    while(vTimestampsImu[first_imu]<=vTimestampsCam[0])
      first_imu++;
    first_imu--; // first imu measurement to be considered

    // Vector for tracking time statistics
    vector<float> vTimesTrack;
    vTimesTrack.resize(tot_images);

    cout << endl << "-------" << endl;
    cout.precision(17);

    /*cout << "Start processing sequence ..." << endl;
    cout << "Images in the sequence: " << nImages << endl;
    cout << "IMU data in the sequence: " << nImu << endl << endl;*/

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    ORB_SLAM3::System SLAM(argv[1],argv[2],ORB_SLAM3::System::IMU_MONOCULAR, false, 0, file_name);
    float imageScale = SLAM.GetImageScale();

    double t_resize = 0.f;
    double t_track = 0.f;

    int proccIm = 0;

    // Main loop
    cv::Mat im;
    vector<ORB_SLAM3::IMU::Point> vImuMeas;
    proccIm = 0;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(8, 8));
    for(int ni=0; ni<nImages; ni++, proccIm++)
    {

      // Read image from file
      im = cv::imread(vstrImageFilenames[ni],cv::IMREAD_GRAYSCALE); //,cv::IMREAD_GRAYSCALE);

      // clahe
      clahe->apply(im,im);

      // cout << "mat type: " << im.type() << endl;
      double tframe = vTimestampsCam[ni];

      if(im.empty())
      {
        cerr << endl << "Failed to load image at: " <<  vstrImageFilenames[ni] << endl;
        return 1;
      }


      // Load imu measurements from previous frame
      vImuMeas.clear();

      if(ni>0)
      {
        // cout << "t_cam " << tframe << endl;

        while(vTimestampsImu[first_imu]<=vTimestampsCam[ni])
        {
          vImuMeas.push_back(
              ORB_SLAM3::IMU::Point(
                vAcc[first_imu].x,
                vAcc[first_imu].y,
                vAcc[first_imu].z,
                vGyro[first_imu].x,
                vGyro[first_imu].y,
                vGyro[first_imu].z,
                vTimestampsImu[first_imu]
                )
              );
          // cout << "t_imu = " << fixed << vImuMeas.back().t << endl;
          first_imu++;
        }
      }

      if(imageScale != 1.f)
      {
#ifdef REGISTER_TIMES
#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t_Start_Resize = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t_Start_Resize = std::chrono::monotonic_clock::now();
#endif
#endif
        int width = im.cols * imageScale;
        int height = im.rows * imageScale;
        cv::resize(im, im, cv::Size(width, height));
#ifdef REGISTER_TIMES
#ifdef COMPILEDWITHC11
        std::chrono::steady_clock::time_point t_End_Resize = std::chrono::steady_clock::now();
#else
        std::chrono::monotonic_clock::time_point t_End_Resize = std::chrono::monotonic_clock::now();
#endif
        t_resize = std::chrono::duration_cast<std::chrono::duration<double,std::milli> >(t_End_Resize - t_Start_Resize).count();
        SLAM.InsertResizeTime(t_resize);
#endif
      }

      // cout << "first imu: " << first_imu[seq] << endl;
      /*cout << "first imu time: " << fixed << vTimestampsImu[first_imu] << endl;
        cout << "size vImu: " << vImuMeas.size() << endl;*/
#ifdef COMPILEDWITHC11
      std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
#else
      std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
#endif

      // Pass the image to the SLAM system
      // cout << "tframe = " << tframe << endl;
      SLAM.TrackMonocular(im,tframe,vImuMeas); // TODO change to monocular_inertial

#ifdef COMPILEDWITHC11
      std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
#else
      std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
#endif

#ifdef REGISTER_TIMES
      t_track = t_resize + std::chrono::duration_cast<std::chrono::duration<double,std::milli> >(t2 - t1).count();
      SLAM.InsertTrackTime(t_track);
#endif

      double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
      ttrack_tot += ttrack;
      // std::cout << "ttrack: " << ttrack << std::endl;

      vTimesTrack[ni]=ttrack;

      // Wait to load the next frame
      double T=0;
      if(ni<nImages-1)
        T = vTimestampsCam[ni+1]-tframe;
      else if(ni>0)
        T = tframe-vTimestampsCam[ni-1];

      if(ttrack<T)
        usleep((T-ttrack)*1e6); // 1e6

    }
    if(seq < num_seq - 1)
    {
      cout << "Changing the dataset" << endl;

      SLAM.ChangeDataset();
    }


    // cout << "ttrack_tot = " << ttrack_tot << std::endl;
    // Stop all threads
    SLAM.Shutdown();


    // Tracking time statistics

    // Save camera trajectory

    if (bFileName)
    {
        const string kf_file =  "kf_" + string(argv[argc-1]) + ".txt";
        const string f_file =  "f_" + string(argv[argc-1]) + ".txt";
        SLAM.SaveTrajectoryEuRoC(f_file);
        SLAM.SaveKeyFrameTrajectoryEuRoC(kf_file);
    }
    else
    {
        SLAM.SaveTrajectoryEuRoC("CameraTrajectory.txt");
        SLAM.SaveKeyFrameTrajectoryEuRoC("KeyFrameTrajectory.txt");
    }

    sort(vTimesTrack.begin(),vTimesTrack.end());
    float totaltime = 0;
    for(int ni=0; ni<nImages; ni++)
    {
        totaltime+=vTimesTrack[ni];
    }
    cout << "-------" << endl << endl;
    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
    cout << "mean tracking time: " << totaltime/proccIm << endl;

    /*const string kf_file =  "kf_" + ss.str() + ".txt";
    const string f_file =  "f_" + ss.str() + ".txt";

    SLAM.SaveTrajectoryEuRoC(f_file);
    SLAM.SaveKeyFrameTrajectoryEuRoC(kf_file);*/

    return 0;
}

void LoadImagesTUMVI(const string &strImagePath, const string &strPathTimes,
                vector<string> &vstrImages, vector<double> &vTimeStamps)
{
    ifstream fTimes(strPathTimes.c_str());
    if (!fTimes.is_open())
    {
      cerr << endl << "Failed to open times file at: "
         <<  strPathTimes << endl;
      return;
    }
    vTimeStamps.reserve(5000);
    vstrImages.reserve(5000);
    while(!fTimes.eof())
    {
        string s;
        getline(fTimes,s);

        if(!s.empty())
        {
            if (s[0] == '#')
                continue;

            int pos = s.find(' ');
            string item = s.substr(0, pos);

            vstrImages.push_back(strImagePath + "/" + item + ".png");
            double t = stod(item);
            vTimeStamps.push_back(t/1e9);
        }
    }
}

void LoadIMU(const string &strImuPath, vector<double> &vTimeStamps, vector<cv::Point3f> &vAcc, vector<cv::Point3f> &vGyro)
{
    ifstream fImu(strImuPath.c_str());
    if (!fImu.is_open())
    {
      cerr << endl << "Failed to open imu file at: "
           <<  strImuPath << endl;
      return;
    }
    vTimeStamps.reserve(5000);
    vAcc.reserve(5000);
    vGyro.reserve(5000);

    while(!fImu.eof())
    {
        string s;
        getline(fImu,s);
        if (s[0] == '#')
            continue;

        if(!s.empty())
        {
            string item;
            size_t pos = 0;
            double data[7];
            int count = 0;
            while ((pos = s.find(',')) != string::npos) {
                item = s.substr(0, pos);
                data[count++] = stod(item);
                s.erase(0, pos + 1);
            }
            item = s.substr(0, pos);
            data[6] = stod(item);

            vTimeStamps.push_back(data[0]/1e9);
            vAcc.push_back(cv::Point3f(data[4],data[5],data[6]));
            vGyro.push_back(cv::Point3f(data[1],data[2],data[3]));
        }
    }
}
