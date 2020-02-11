import os
import yaml

class PathPointsStuffer:
    def __init__(self, paths_loc):
        self.FOLDER_W_PATHS_LOCATION = paths_loc
        self.path_name = ""
        self.point_to_send_index = 0
        self.path_points = ""

    def get_path_location(self, path_nam):
        if os.path.isfile(self.FOLDER_W_PATHS_LOCATION + path_nam + ".yaml"):
            return self.FOLDER_W_PATHS_LOCATION + path_nam + ".yaml"
        return ""

    def set_path_name(self, path_nam):
        self.path_name = self.get_path_location(path_nam)
        if self.path_name == "":
            return False
        return True

    def get_path_points_from_yaml(self):
        try:
            with open(self.path_name) as reader:
                if os.stat(self.path_name).st_size != 0:
                    self.path_points = dict(yaml.safe_load(reader))
        except IOError:
            reader = open(self.path_name, "w")
        reader.close()

    def fill_new_msg_fix(self, msg):
        if self.point_to_send_index < len(self.path_points.keys()):
            msg.latitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][0]
            msg.longitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][1]
            msg.altitude = self.path_points["pose"+str(self.point_to_send_index)]["position"][2]
            self.point_to_send_index = self.point_to_send_index + 1
            return msg

    def are_path_points_availible(self):
        return self.point_to_send_index < len(self.path_points.keys())
