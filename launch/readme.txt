W celu wyjscia ze stanu standby maszyna stanu oczekuje na polaczenie z aplikacja mobilna. Na ten moment realizowane jest to z wykorzystaniem rostopic pub:

rostopic pub /agribot/app_commands_events agribot_commander/mobile_app_frame "header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: ''
can_id: 10
can_byte_array: [10,0,0,0,0,0,0,0]"

Jesli chcemy przejsc ze stanu manualnego na automatyczny nalezy wykorzystac ta ramke

rostopic pub /agribot/app_commands_events agribot_commander/mobile_app_frame "header:
  seq: 0
  stamp:
    secs: 0
    nsecs: 0
  frame_id: ''
can_id: 30
can_byte_array: [10,0,0,0,0,0,0,0]"

Poruszanie sie robota z uzyciem teleop powoduje wykywanie stanu EXECUTE_MOVE_COMMANDS