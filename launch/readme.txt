W celu wyjscia ze stanu standby maszyna stanu oczekuje na polaczenie z aplikacja mobilna. Na ten moment realizowane jest to z wykorzystaniem rostopic pub:

rostopic pub /agribot/mobile_app_cmunication agribot_msgs/AgribotMobileAppFrame "header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: ''
can_id: 10
can_byte_array: "[0,0,0,0,0,0,0,0]""

Jesli chcemy przejsc ze stanu manualnego na automatyczny nalezy ustawic nazwe taska z uzyciem topica

rostopic pub /agribot/chosen_task_name std_msgs/String "data: 'taskname'"

i wykorzystac ta ramke

rostopic pub /agribot/mobile_app_cmunication agribot_msgs/AgribotMobileAppFrame "header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: ''
can_id: 31
can_byte_array: "[0,0,0,0,0,0,0,0]""

Poruszanie sie robota z uzyciem teleop powoduje wykywanie stanu EXECUTE_MOVE_COMMANDS