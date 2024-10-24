#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <unordered_map>
#include "json.hpp"   // Please look into READ.me document about the license 

using json = nlohmann::json;

// Define the structure and functions directly in the test file

struct ClusteredObject {
    int f_id;
    std::string f_timestamp;
    std::vector<std::vector<double>> cluster_data;
};

double calculateDistance(double x1, double y1, double x2, double y2) {
    return std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

std::vector<ClusteredObject> clusterSensorData(const json& sensorData) {
    std::unordered_map<std::string, ClusteredObject> clusters;
    int f_id_counter = 1;

    for (const auto& entry : sensorData) {
        for (const auto& sensor : entry.items()) {
            const auto& sensor_info = sensor.value();
            std::string timestamp = sensor_info["timestamp"];
            auto object_positions = sensor_info["object_positions_x_y"];
            auto object_ids = sensor_info["object_ids"];

            for (size_t i = 0; i < object_positions.size(); ++i) {
                double x = object_positions[i][0];
                double y = object_positions[i][1];

                bool clustered = false;
                for (auto& cluster : clusters) {
                    const auto& cluster_obj = cluster.second;
                    for (const auto& cluster_data : cluster_obj.cluster_data) {
                        double cluster_x = cluster_data[0];
                        double cluster_y = cluster_data[1];

                        if (calculateDistance(x, y, cluster_x, cluster_y) <= 2.0) {
                            cluster.second.cluster_data.push_back({ x, y, std::stod(sensor.key()) });
                            clustered = true;
                            break;
                        }
                    }
                    if (clustered) { break; }
                }

                if (!clustered) {
                    ClusteredObject new_cluster;
                    new_cluster.f_id = f_id_counter++;
                    new_cluster.f_timestamp = timestamp;
                    new_cluster.cluster_data.push_back({ x, y, std::stod(sensor.key()) });
                    clusters[timestamp + std::to_string(new_cluster.f_id)] = new_cluster;
                }
            }
        }
    }

    std::vector<ClusteredObject> result;
    for (const auto& entry : clusters) {
        result.push_back(entry.second);
    }

    return result;
}

// Test for the calculateDistance function
void testCalculateDistance() {
    assert(calculateDistance(0, 0, 3, 4) == 5.0); // 3-4 triangle
    assert(calculateDistance(1, 1, 1, 1) == 0.0); // Same point
    assert(calculateDistance(-1, -1, 1, 1) == std::sqrt(8)); // Diagonal
    assert(calculateDistance(-5, -5, 5, 5) == std::sqrt(200)); // Larger diagonal
    std::cout << "All tests for calculateDistance passed!" << std::endl;
}

// Test for the clusterSensorData function
void testClusterSensorData() {
    // Prepare some mock sensor data
    json testData = {
        {"sensor_1", {
            {"timestamp", "2024-10-23 11:02:08.101"},
            {"object_positions_x_y", {{{0.0, 0.0}}, {{3.0, 4.0}}, {{5.0, 5.0}}}},
            {"object_ids", {1, 2, 3}}
        }},
        {"sensor_2", {
            {"timestamp", "2024-10-23 11:02:08.102"},
            {"object_positions_x_y", {{{0.5, 0.5}}, {{6.0, 6.0}}}},
            {"object_ids", {4, 5}}
        }}
    };

    auto clusteredData = clusterSensorData(testData);
    
    // number of clusters formed
    assert(clusteredData.size() > 0); // At least one cluster should be formed
    std::cout << "Clustering test passed! Number of clusters: " << clusteredData.size() << std::endl;
}

// Main function to run tests
int main() {
    testCalculateDistance();
    testClusterSensorData();

    std::cout << "All tests completed successfully." << std::endl;
    return 0;
}
