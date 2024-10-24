#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include "json.hpp"


using json= nlohmann::json;


struct ClusteredObject
{
    int f_id;
    std::string f_timestamp;
    std::vector<std::vector<double>> cluster_data;
};


double calculateDistance(double x1, double y1, double x2, double y2)
{
    return std::sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
}




std::vector<ClusteredObject> clusterSensorData(const json& sensorData)
{
    std::unordered_map<std::string, ClusteredObject> clusters;
    int f_id_counter =1;

    for(const auto& entry : sensorData)
    {
        for(const auto& sensor : entry.items())
        {
            const auto& sensor_info = sensor.value();
            std::string timestamp = sensor_info["timestamp"];
            auto object_positions = sensor_info["object_positions_x_y"];
            auto object_ids = sensor_info["object_ids"];

            for(size_t i =0; i< object_positions.size(); ++i)
            {
                double x = object_positions[i][0];
                double y = object_positions[i][1];

                bool clustered = false;
                for(auto& cluster : clusters)
                {
                    const auto& cluster_obj = cluster.second;
                    for(const auto& cluster_data: cluster_obj.cluster_data)
                    {
                        double cluster_x = cluster_data[0];
                        double cluster_y = cluster_data[1];

                        if(calculateDistance(x, y, cluster_x, cluster_y) <=2.0)
                        {
                            cluster.second.cluster_data.push_back({x, y, std::stod(sensor.key())});
                            clustered = true;
                            break;
                        }
                    }
                    if(clustered) { break;}
                }

                if(!clustered)
                {
                        ClusteredObject new_cluster;
                        new_cluster.f_id = f_id_counter++ ;
                        new_cluster.f_timestamp = timestamp;
                        new_cluster.cluster_data.push_back({x, y, std::stod(sensor.key())});
                        clusters[timestamp + std::to_string(new_cluster.f_id)] = new_cluster;

                }

            }
        }
    }

    std::vector<ClusteredObject> result;
    for(const auto& entry : clusters)
    {
        result.push_back(entry.second);
    }

    return result;
}




void writeCSVfile(const std::vector<ClusteredObject>& clusteredData, const std::string& filename){
 
 std::ofstream file(filename);

if(file.is_open())
{
    file << "f_timestamp, f_id, cluster_data \n";
    for(const auto& cluster : clusteredData)
    {
        std::stringstream ss;
        for(const auto& data : cluster.cluster_data)
        {
             ss<< "["<< data[0]<<","<<data[1]<<","<<data[2]<<"], ";

        }
        std:: string cluster_data_str = ss.str();
        cluster_data_str = cluster_data_str.substr(0, cluster_data_str.size()-2);

        file << cluster.f_timestamp << "," <<cluster.f_id <<"," << cluster_data_str <<"\n";          
    }
    file.close();
}
else {
    std::cerr<< "Could not open file for writing the data"<<std::endl;
}
}

int main()
{

std::ifstream inputFile("task_data.json");
json sensorData;

if(inputFile.is_open())
{
    inputFile >> sensorData;
    inputFile.close();
}
else{

    std::cerr << "JSON file could not be opened" << std::endl;
    return 1;
}


auto clusteredData = clusterSensorData(sensorData);

writeCSVfile(clusteredData, "C:\\Users\\prath\\Assignment_Sentics\\S_Sensor_Fusion_App\\build\\Release\\output.csv");



    std::cout <<"The Data processing is complete and the final result is saved as ouput.csv" << std::endl;
    return 0;
}
