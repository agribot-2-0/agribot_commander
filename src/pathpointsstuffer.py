import firebase_admin
from firebase_admin import credentials, storage
import os
import xml.etree.ElementTree as ET
import re
import math
import numpy as np
import yaml

class PathPointsStuffer:
    FOLDER_W_PATHS_LOCATION = "" #location of paths folder inside agribot_map package
    path_name = ""

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

    def transform_to_robot_coordinates(self):
        if(len(self.path_points.keys()) < 0):
            return False
        base_lon = float(self.path_points["pose0"]["position"][0])
        base_lat = float(self.path_points["pose0"]["position"][1])
        for i in range(0, len(self.path_points.keys())):
            self.path_points["pose"+str(i)]["position"][0] = (float(self.path_points["pose"+str(i)]["position"][0]) - base_lon)*10000
            self.path_points["pose"+str(i)]["position"][1] = (float(self.path_points["pose"+str(i)]["position"][1]) - base_lat)*10000
        print(self.path_points)

    def get_path_points_from_yaml(self):
        try:
            with open(self.path_name) as file:
                if (os.stat(self.path_name).st_size!=0):
                    self.path_points = dict(yaml.safe_load(file))
        except IOError:
            file = open(self.path_name, "w")
        file.close()

