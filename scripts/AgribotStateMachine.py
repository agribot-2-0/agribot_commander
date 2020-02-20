#!/usr/bin/env python
import rospy
import smach
import smach_ros
import roslib
from State_Standby import Standby
from State_EngineStop import EngineStop
from State_CheckMobileAppConnection import CheckMobileAppConnection
from State_CheckSystemHealth import CheckSystemHealth
from State_ReadData import ReadData
from State_SendData import SendData
from State_LogsAndProtocolData import LogsAndProtocolData
from State_CheckIfBlockedDirection import CheckIfBlockedDirection
from State_CreateOutputInfoLogs import CreateOutputInfoLogs
from State_AutonomousControlTransition import AutonomousControlTransition
from State_IsTaskChoosen import IsTaskChosen
from State_ChooseTask import ChooseTask
from State_AutonomousTaskMonitor import AutonomousTaskMonitor
from State_TaskCompletionRaport import TaskCompletionRaport
from State_SafeState_Start import SafeStateStart
from State_ManualControl_Start import ManualControlStart
from State_MobileAppRoutine_Start import MobileAppRoutineStart
from State_AutonomousControl_Start import AutonomousControlStart
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

smach.set_loggers(rospy.logdebug,
                  rospy.logwarn,
                  rospy.logdebug,
                  rospy.logerr)

def main():
    rospy.init_node('agribot_state_machine')
    # Create an agribot SMACH state machine
    sm_agribot = smach.StateMachine(outcomes=['tr_agribot_offline'])

    sm_agribot.userdata.ag_safestate_in = 'system_power_up'
    sm_agribot.userdata.ag_auto_in = ''
    sm_agribot.userdata.ag_manual_in = 'app_input_received'

    rospy.set_param('/agribot/current_task_name', '')

    # Open the container
    with sm_agribot:
        # SAFE STATE
        # Create the sub SMACH state machine
        sm_safe_state = smach.StateMachine(outcomes=['tr_application_input_received',
                                                     'tr_system_repaired',
                                                     'tr_system_power_is_off'],
                                           input_keys=['sm_safestate_in'],
                                           output_keys=['sm_safestate_out_auto',
                                                        'sm_safestate_out_manual'])
        sm_safe_state.userdata.sm_safestate_in='system_power_up'
        # Open the container
        with sm_safe_state:
            # Add states to the container
            smach.StateMachine.add('SAFE_STATE_START', SafeStateStart(),
                                   transitions={'tr_in_system_power_up':'STANDBY',
                                                'tr_in_return_to_safe_state':'ENGINE_STOP'},
                                   remapping={'safe_state_reason':'sm_safestate_in'})
            smach.StateMachine.add('STANDBY', Standby(),
                                   transitions={'tr_app_input_received_standby':'tr_application_input_received',
                                                'tr_default_on_standby':'STANDBY'},
                                   remapping={'safe_state_reason_man_out':'sm_safestate_out_manual',
                                              'safe_state_reason_auto_out':'sm_safestate_out_auto'})
            smach.StateMachine.add('ENGINE_STOP', EngineStop(),
                                   transitions={'tr_engine_stopped_normal':'STANDBY',
                                                'tr_engine_stop_failure_reaction':'tr_system_power_is_off'},
                                   remapping={'engine_stop_reason':'sm_safestate_in'})
        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('SAFE_STATE', sm_safe_state,
                               transitions={'tr_application_input_received':'MANUAL_CONTROL_STATE',
                                            'tr_system_repaired':'tr_agribot_offline',
                                            'tr_system_power_is_off':'tr_agribot_offline'},
                               remapping={'sm_safestate_in':'ag_safestate_in',
                                          'sm_safestate_out_auto':'ag_auto_in',
                                          'sm_safestate_out_manual':'ag_manual_in'})
        #MANUAL CONTROL STATE
        # Create the sub SMACH state machine
        sm_man_ctrl = smach.StateMachine(outcomes=['tr_auto_ctrl_prepared', 'tr_man_failed_connection'],
                                         input_keys=['sm_manual_in'],
                                         output_keys=['sm_manual_out_safe',
                                                      'sm_manual_out_auto'])
        with sm_man_ctrl:
             # Add states to the container
            smach.StateMachine.add('MANUAL_CONTROL_START', ManualControlStart(),
                                   transitions={'tr_task_config_error':'MOBILE_APP_CONTROL_ROUTINE',
                                                'tr_app_input_received':'MOBILE_APP_CONTROL_ROUTINE'},
                                   remapping={'manual_control_state_reason':'sm_manual_in',
                                              'manual_control_state_app_reason':'sm_mobile_app_reason_in'})
            #MOBILE APP CONTROL ROUTINE
            # Create the sub SMACH state machine
            sm_mobile_app_control_routine = smach.StateMachine(outcomes=['tr_automatic_control_command',
                                                                         'tr_received_only_move_commands', 
                                                                         'tr_mob_app_failed_connection'],
                                                               input_keys=['sm_mobile_app_reason_in'],
                                                               output_keys=['sm_mobile_app_out_safe',
                                                                            'sm_mobile_app_out_auto'])
            sm_mobile_app_control_routine.userdata.sm_mobile_app_reason_out = 'automatic_control_command'
            with sm_mobile_app_control_routine:
                # Add states to the container
                smach.StateMachine.add('MOBILE_APP_CONTROL_START', MobileAppRoutineStart(),
                                       transitions={'tr_task_config_error':'CHECK_MOBILE_APP_CONNECTION',
                                                    'tr_app_input_received':'CHECK_MOBILE_APP_CONNECTION'},
                                       remapping={'mobile_app_routine_reason_in':'sm_mobile_app_reason_in'})
                smach.StateMachine.add('CHECK_MOBILE_APP_CONNECTION', CheckMobileAppConnection(),
                                       transitions={'tr_connection_is_stable':'READ_DATA',
                                                    'tr_connection_is_unstable':'tr_mob_app_failed_connection'},
                                       remapping={'connection_check_out':'sm_mobile_app_out_safe'})
                smach.StateMachine.add('READ_DATA', ReadData(),
                                       transitions={'tr_send_data_def_event':'CHECK_MOBILE_APP_CONNECTION',
                                                    'tr_auto_ctrl_command':'tr_automatic_control_command',
                                                    'tr_rec_only_move_commands':'tr_received_only_move_commands'},
                                       remapping={'read_data_command_out':'sm_mobile_app_out_auto'})
            #Finish state machine transitions and add it to agribot_state_machine
            smach.StateMachine.add('MOBILE_APP_CONTROL_ROUTINE', sm_mobile_app_control_routine,
                                   transitions={'tr_automatic_control_command':'AUTONOMOUS_CONTROL_TRANSITION',
                                                'tr_received_only_move_commands':'EXECUTE_MOVE_COMMANDS',
                                                'tr_mob_app_failed_connection':'tr_man_failed_connection'},
                                   remapping={'sm_mobile_app_out_safe':'sm_manual_out_safe',
                                              'sm_mobile_app_out_auto':'sm_manual_out_auto'})
            #EXECUTE MOVE COMMANDS ROUTINE
            # Create the sub SMACH state machine
            sm_execute_move_commands = smach.StateMachine(outcomes=['tr_executed_move_commands'])
            with sm_execute_move_commands:
                # Add states to the container
                smach.StateMachine.add('CHECK_IF_BLOCKED_DIRECTION', CheckIfBlockedDirection(),
                                       transitions={'tr_no_obstacles':'CREATE_INFO_LOGS'})
                smach.StateMachine.add('CREATE_INFO_LOGS', CreateOutputInfoLogs(),
                                       transitions={'tr_executed_outputs':'tr_executed_move_commands'})
            #Finish state machine transitions and add it to agribot_state_machine
            smach.StateMachine.add('EXECUTE_MOVE_COMMANDS', sm_execute_move_commands,
                                   transitions={'tr_executed_move_commands':'MOBILE_APP_CONTROL_ROUTINE'})
            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_TRANSITION', AutonomousControlTransition(),
                                   transitions={'tr_auto_mode_failed':'MOBILE_APP_CONTROL_ROUTINE',
                                                'tr_auto_control_prepared':'tr_auto_ctrl_prepared',
                                                'tr_in_progress':'AUTONOMOUS_CONTROL_TRANSITION'},
                                   remapping={'auto_control_transition_out':'sm_manual_out_auto'})
        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('MANUAL_CONTROL_STATE', sm_man_ctrl,
                               transitions={'tr_auto_ctrl_prepared':'AUTONOMOUS_CONTROL_STATE', 
                                            'tr_man_failed_connection':'SAFE_STATE'},
                               remapping={'sm_manual_in':'ag_manual_in',
                                          'sm_manual_out_safe':'ag_safestate_in',
                                          'sm_manual_out_auto':'ag_auto_in'})
        #AUTONOMOUS CONTROL STATE
        # Create the sub SMACH state machine
        sm_auto_ctrl = smach.StateMachine(outcomes=['tr_task_configuration_failed', 'tr_return_to_save_state'],
                                          input_keys=['sm_auto_in'],
                                          output_keys=['sm_auto_out_man',
                                                       'sm_auto_out_safe'])
        with sm_auto_ctrl:
            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_START', AutonomousControlStart(),
                                   transitions={'tr_auto_control_prepared':'IS_TASK_CHOSEN',
                                                'tr_sys_repaired':'AUTONOMOUS_TASK_MONITOR'},
                                   remapping={'autonomous_control_state_reason':'sm_auto_in'})
            smach.StateMachine.add('IS_TASK_CHOSEN', IsTaskChosen(),
                                   transitions={'tr_task_is_active':'AUTONOMOUS_TASK_MONITOR',
                                                'tr_no_active_task':'CHOOSE_TASK'})
            smach.StateMachine.add('CHOOSE_TASK', ChooseTask(),
                                   transitions={'tr_task_is_active':'AUTONOMOUS_TASK_MONITOR',
                                                'tr_task_conf_error':'tr_task_configuration_failed'},
                                   remapping={'choose_task_output':'sm_auto_out_man'})
            smach.StateMachine.add('AUTONOMOUS_TASK_MONITOR', AutonomousTaskMonitor(),
                                   transitions={'tr_succes':'TASK_COMPLETION_RAPORT'})
            smach.StateMachine.add('TASK_COMPLETION_RAPORT', TaskCompletionRaport(),
                                   transitions={'tr_complete':'tr_return_to_save_state'},
                                   remapping={'task_completion_output':'sm_auto_control_reason_out'})
         #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('AUTONOMOUS_CONTROL_STATE', sm_auto_ctrl,
                               transitions={'tr_task_configuration_failed':'MANUAL_CONTROL_STATE',
                                            'tr_return_to_save_state':'SAFE_STATE'},
                               remapping={'sm_auto_in':'ag_auto_in',
                                          'sm_auto_out_man':'ag_manual_in',
                                          'sm_auto_out_safe':'ag_safe_in'})
    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('agribot_state_machine_intro_server', sm_agribot, '/SM_ROOT')
    sis.start()
    # Execute SMACH plan
    outcome = sm_agribot.execute()
    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()