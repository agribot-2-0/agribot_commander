import firebase_admin
from firebase_admin import credentials, storage
import os
import xml.etree.ElementTree as ET
import re
import math
import numpy as np
import yaml
import matplotlib.pyplot as plt

class PathPointsStuffer:
    FOLDER_W_PATHS_LOCATION = "" #location of paths folder inside agribot_map package
    path_name = ""
    point_to_send_index = 0
    alpha = 0.0
    earth_radius = 6371000 #[m]


    def __init__(self, paths_loc):
        self.FOLDER_W_PATHS_LOCATION = paths_loc

    def get_path_location(self, path_nam):
        if(os.path.isfile(self.FOLDER_W_PATHS_LOCATION + path_nam + ".yaml")):
            return self.FOLDER_W_PATHS_LOCATION + path_nam + ".yaml"
        else:
            return ""

    def set_path_name(self, path_nam):
        self.path_name = self.get_path_location(path_nam)
        if(self.path_name == ""):
            return False
        return True

    def lon_lat_to_x_y(self, lon, lat):
        X = self.earth_radius*math.cos(lat)*math.cos(lon)
        Y = self.earth_radius*math.cos(lat)*math.sin(lon)
        return X, Y

    def transform_to_robot_coordinates(self):
        X_vec = []
        Y_vec = []
        if(len(self.path_points.keys()) < 0):
            return False
        X_base, Y_base = self.lon_lat_to_x_y(lon = float(self.path_points["pose0"]["position"][0]), lat =float(self.path_points["pose0"]["position"][1]))
        for i in range(0, len(self.path_points.keys())):
            X, Y = self.lon_lat_to_x_y(lon = float(self.path_points["pose"+str(i)]["position"][0]), lat =float(self.path_points["pose"+str(i)]["position"][1]))
            self.path_points["pose"+str(i)]["position"][0] = X - X_base
            self.path_points["pose"+str(i)]["position"][1] = Y - Y_base
            X_vec.append(X - X_base)
            Y_vec.append(Y - Y_base)
        #plt.plot(X_vec,Y_vec)
        #plt.show()

    def get_path_points_from_yaml(self):
        try:
            with open(self.path_name) as file:
                if (os.stat(self.path_name).st_size!=0):
                    self.path_points = dict(yaml.safe_load(file))
        except IOError:
            file = open(self.path_name, "w")
        file.close()

    def fill_new_msg(self, msg):
        if (self.point_to_send_index < len(self.path_points.keys())):
            msg.target_pose.pose.position.x = self.path_points["pose"+str(self.point_to_send_index)]["position"][0]
            msg.target_pose.pose.position.y = self.path_points["pose"+str(self.point_to_send_index)]["position"][1]
            msg.target_pose.pose.position.z = 0.0
            msg.target_pose.pose.orientation.x = 0.0
            msg.target_pose.pose.orientation.y = 0.0
            msg.target_pose.pose.orientation.z = 0.0
            msg.target_pose.pose.orientation.w = 0.0
            self.point_to_send_index = self.point_to_send_index + 1
            return msg

    def fill_new_msg_fix(self, msg):
        if (self.point_to_send_index < len(self.path_points.keys())):
            msg.latitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][0]
            msg.longitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][1]
            msg.altitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][2]
            self.point_to_send_index = self.point_to_send_index + 1
            return msg

    def are_path_points_availible(self):
        if (self.point_to_send_index < len(self.path_points.keys())):
            return True
        else:
            return False
