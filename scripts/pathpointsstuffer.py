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

    def get_path_points_from_yaml(self):
        try:
            with open(self.path_name) as file:
                if (os.stat(self.path_name).st_size!=0):
                    self.path_points = dict(yaml.safe_load(file))
        except IOError:
            file = open(self.path_name, "w")
        file.close()

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
