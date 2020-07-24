import os
import yaml


class PathPointsStuffer(object):
    def __init__(self, paths_loc):
        self.folder_w_paths_location = paths_loc
        self.path_name = ""
        self.point_to_send_index = 0
        self.path_points = ""

    def get_path_location(self, path_nam):
        if os.path.isfile(self.folder_w_paths_location + path_nam + ".yaml"):
            return self.folder_w_paths_location + path_nam + ".yaml"
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
            pose_str = "pose"+str(self.point_to_send_index)
            position = self.path_points[pose_str]["position"]
            orientation = self.path_points[pose_str]["orientation"]
            msg.latitude = position[0]
            msg.longitude = position[1]
            msg.altitude = position[2]
            msg.orientation.x = orientation[0]
            msg.orientation.y = orientation[1]
            msg.orientation.z = orientation[2]
            msg.orientation.w = orientation[3]
            self.point_to_send_index += 1
            return msg
        return None

    def are_path_points_availible(self):
        return self.point_to_send_index < len(self.path_points.keys())

    def reset_path_point_stuffer(self):
        self.path_name = ""
        self.point_to_send_index = 0
        self.path_points = ""
