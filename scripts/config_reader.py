import os
import yaml

class ConfigReader(object):
    def __init__(self, tasks_path):
        self.task_name = ''
        self.tasks_directory_location = tasks_path
        self.task_conf = ''
        self.path_name_from_conf = ''

    def get_task_location(self, task_name):
        if os.path.isfile(self.tasks_directory_location + task_name + ".yaml"):
            return self.tasks_directory_location + task_name + ".yaml"
        return ""

    def set_task_name(self, task_name):
        self.task_name = self.get_task_location(task_name)
        if self.task_name == "":
            return False
        return True

    def get_task_conf_from_yaml(self):
        try:
            with open(self.task_name) as reader:
                if os.stat(self.task_name).st_size != 0:
                    self.task_conf = dict(yaml.safe_load(reader))
                    if 'task' in self.task_conf and 'path_name' in self.task_conf['task']:
                        self.path_name_from_conf = self.task_conf['task']['path_name'][0]
        except IOError:
            reader = open(self.task_name, "w")
        reader.close()
        if self.path_name_from_conf == '':
            return False
        return True

    def return_tasks_path_name(self):
        return self.path_name_from_conf
