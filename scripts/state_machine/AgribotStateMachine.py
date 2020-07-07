#!/usr/bin/env python
import rospy
import smach
import smach_ros
import roslib
from state_machine.safe_state.State_Standby import Standby
from state_machine.safe_state.State_EngineStop import EngineStop
from state_machine.manual_control.State_MobileAppConnectionRoutine import MobileAppConnectionRoutine
from state_machine.manual_control.State_AutonomousControlTransition import \
        AutonomousControlTransition
from state_machine.autonomous_control.State_IsTaskChoosen import IsTaskChosen
from state_machine.autonomous_control.State_ChooseTask import ChooseTask
from state_machine.autonomous_control.State_AutonomousTaskMonitor import AutonomousTaskMonitor
from state_machine.autonomous_control.State_TaskCompletionRaport import TaskCompletionRaport
from state_machine.safe_state.State_SafeState_Start import SafeStateStart
from state_machine.autonomous_control.State_TaskIsBlocked import TaskIsBlocked
from state_machine.manual_control.State_ManualControl_Start import ManualControlStart
from state_machine.manual_control.State_MobileAppRoutine_Start import MobileAppRoutineStart
from state_machine.autonomous_control.State_AutonomousControl_Start import AutonomousControlStart
roslib.load_manifest('smach')
roslib.load_manifest('smach_ros')

# if rebuilding or adding new elements to the StateMachine change the last logger as logerr
smach.set_loggers(rospy.logdebug,
                  rospy.logwarn,
                  rospy.logdebug,
                  rospy.logdebug)

def main():
    # pylint: disable=too-many-statements
    rospy.init_node('agribot_state_machine')
    # Create an agribot SMACH state machine
    sm_agribot = smach.StateMachine(outcomes=['tr_agribot_offline'])

    sm_agribot.userdata.ag_safestate_in = 'system_power_up'
    sm_agribot.userdata.ag_auto_in = ''
    sm_agribot.userdata.ag_manual_in = 'app_input_received'
    rospy.set_param('/agribot/current_task_name', '')
    rospy.set_param('/agribot/robot_current_state', 'safe_state')

    # Open the container
    with sm_agribot:
        # SAFE STATE
        # Create the sub SMACH state machine
        sm_safe_state = smach.StateMachine(outcomes=['tr_application_input_received',
                                                     'tr_system_repaired',
                                                     'tr_autonomous_start',
                                                     'tr_system_power_is_off'],
                                           input_keys=['sm_safestate_in'],
                                           output_keys=['sm_safestate_out_auto',
                                                        'sm_safestate_out_manual'])
        sm_safe_state.userdata.sm_safestate_in = 'system_power_up'
        # Open the container
        with sm_safe_state:
            # Add states to the container
            smach.StateMachine.add('SAFE_STATE_START', SafeStateStart(),
                                   transitions={'tr_in_system_power_up':
                                                'STANDBY',
                                                'tr_in_return_to_safe_state':
                                                'ENGINE_STOP'},
                                   remapping={'safe_state_reason':
                                              'sm_safestate_in'})
            smach.StateMachine.add('STANDBY', Standby(),
                                   transitions={'tr_app_input_received_standby':
                                                'tr_application_input_received',
                                                'tr_autonomous_config_standby':
                                                'tr_autonomous_start'},
                                   remapping={'safe_state_reason_man_out':
                                              'sm_safestate_out_manual',
                                              'safe_state_reason_auto_out':
                                              'sm_safestate_out_auto'})
            smach.StateMachine.add('ENGINE_STOP', EngineStop(),
                                   transitions={'tr_engine_stopped_normal':
                                                'STANDBY',
                                                'tr_engine_stop_failure_reaction':
                                                'tr_system_power_is_off'},
                                   remapping={'engine_stop_reason':'sm_safestate_in'})
        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('SAFE_STATE', sm_safe_state,
                               transitions={'tr_application_input_received':
                                            'MANUAL_CONTROL_STATE',
                                            'tr_system_repaired':
                                            'tr_agribot_offline',
                                            'tr_system_power_is_off':
                                            'tr_agribot_offline',
                                            'tr_autonomous_start':
                                            'AUTONOMOUS_CONTROL_STATE'},
                               remapping={'sm_safestate_in':
                                          'ag_safestate_in',
                                          'sm_safestate_out_auto':
                                          'ag_auto_in',
                                          'sm_safestate_out_manual':
                                          'ag_manual_in'})
        #MANUAL CONTROL STATE
        # Create the sub SMACH state machine
        sm_man_ctrl = smach.StateMachine(outcomes=['tr_auto_ctrl_prepared',
                                                   'tr_man_failed_connection'],
                                         input_keys=['sm_manual_in'],
                                         output_keys=['sm_manual_out_safe',
                                                      'sm_manual_out_auto'])

        sm_man_ctrl.userdata.sm_manual_out_safe = ''
        with sm_man_ctrl:
             # Add states to the container
            smach.StateMachine.add('MANUAL_CONTROL_START', ManualControlStart(),
                                   transitions={'tr_task_config_error':
                                                'MOBILE_APP_CONTROL_ROUTINE',
                                                'tr_app_input_received':
                                                'MOBILE_APP_CONTROL_ROUTINE'},
                                   remapping={'manual_control_state_reason':
                                              'sm_manual_in',
                                              'manual_control_state_app_reason':
                                              'sm_mobile_app_reason_in'})
            #MOBILE APP CONTROL ROUTINE
            # Create the sub SMACH state machine
            sm_mobile_app_control_routine = \
            smach.StateMachine(outcomes=['tr_automatic_control_command',
                                         'tr_mob_app_failed_connection'],
                               input_keys=['sm_mobile_app_reason_in'],
                               output_keys=['sm_mobile_app_out_safe',
                                            'sm_mobile_app_out_auto'])
            sm_mobile_app_control_routine.userdata.sm_mobile_app_reason_out = \
            'automatic_control_command'
            sm_mobile_app_control_routine.userdata.sm_mobile_app_out_safe = ''
            with sm_mobile_app_control_routine:
                # Add states to the container
                smach.StateMachine.add('MOBILE_APP_CONTROL_START', MobileAppRoutineStart(),
                                       transitions={'tr_task_config_error':
                                                    'MOBILE_APP_CONNECTION_ROUTINE',
                                                    'tr_app_input_received':
                                                    'MOBILE_APP_CONNECTION_ROUTINE'},
                                       remapping={'mobile_app_routine_reason_in':
                                                  'sm_mobile_app_reason_in'})
                smach.StateMachine.add('MOBILE_APP_CONNECTION_ROUTINE',
                                       MobileAppConnectionRoutine(),
                                       transitions={'tr_connection_is_unstable':
                                                    'tr_mob_app_failed_connection',
                                                    'tr_auto_ctrl_command':
                                                    'tr_automatic_control_command'},
                                       remapping={'connection_check_out':
                                                  'sm_mobile_app_out_safe',
                                                  'read_data_command_out':
                                                  'sm_mobile_app_out_auto'})
            #Finish state machine transitions and add it to agribot_state_machine
            smach.StateMachine.add('MOBILE_APP_CONTROL_ROUTINE', sm_mobile_app_control_routine,
                                   transitions={'tr_automatic_control_command':
                                                'AUTONOMOUS_CONTROL_TRANSITION',
                                                'tr_mob_app_failed_connection':
                                                'tr_man_failed_connection'},
                                   remapping={'sm_mobile_app_out_safe':
                                              'sm_manual_out_safe',
                                              'sm_mobile_app_out_auto':
                                              'sm_manual_out_auto'})

            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_TRANSITION', AutonomousControlTransition(),
                                   transitions={'tr_auto_mode_failed':
                                                'MOBILE_APP_CONTROL_ROUTINE',
                                                'tr_auto_control_prepared':
                                                'tr_auto_ctrl_prepared',
                                                'tr_in_progress':
                                                'AUTONOMOUS_CONTROL_TRANSITION'},
                                   remapping={'auto_control_transition_out':
                                              'sm_manual_out_auto'})
        #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('MANUAL_CONTROL_STATE', sm_man_ctrl,
                               transitions={'tr_auto_ctrl_prepared':
                                            'AUTONOMOUS_CONTROL_STATE',
                                            'tr_man_failed_connection':
                                            'SAFE_STATE'},
                               remapping={'sm_manual_in':
                                          'ag_manual_in',
                                          'sm_manual_out_safe':
                                          'ag_safestate_in',
                                          'sm_manual_out_auto':
                                          'ag_auto_in'})
        #AUTONOMOUS CONTROL STATE
        # Create the sub SMACH state machine
        sm_auto_ctrl = smach.StateMachine(outcomes=['tr_task_configuration_failed',
                                                    'tr_return_to_save_state',
                                                    'tr_task_suspended'],
                                          input_keys=['sm_auto_in'],
                                          output_keys=['sm_auto_out_man',
                                                       'sm_auto_out_safe'])
        sm_auto_ctrl.userdata.sm_auto_out_safe = ''
        with sm_auto_ctrl:
            # Add states to the container
            smach.StateMachine.add('AUTONOMOUS_CONTROL_START', AutonomousControlStart(),
                                   transitions={'tr_auto_control_prepared':
                                                'IS_TASK_CHOSEN',
                                                'tr_sys_repaired':
                                                'AUTONOMOUS_TASK_MONITOR'},
                                   remapping={'autonomous_control_state_reason':
                                              'sm_auto_in'})
            smach.StateMachine.add('IS_TASK_CHOSEN', IsTaskChosen(),
                                   transitions={'tr_task_is_active':
                                                'AUTONOMOUS_TASK_MONITOR',
                                                'tr_no_active_task':
                                                'CHOOSE_TASK'})
            smach.StateMachine.add('CHOOSE_TASK', ChooseTask(),
                                   transitions={'tr_task_is_active':
                                                'AUTONOMOUS_TASK_MONITOR',
                                                'tr_task_conf_error':
                                                'tr_task_configuration_failed'},
                                   remapping={'choose_task_output':
                                              'sm_auto_out_man'})
            smach.StateMachine.add('AUTONOMOUS_TASK_MONITOR', AutonomousTaskMonitor(),
                                   transitions={'tr_succes':
                                                'TASK_COMPLETION_RAPORT',
                                                'tr_danger':
                                                'TASK_IS_BLOCKED'})
            smach.StateMachine.add('TASK_IS_BLOCKED', TaskIsBlocked(),
                                   transitions={'tr_continue_task':
                                                'AUTONOMOUS_TASK_MONITOR',
                                                'tr_human_intervention':
                                                'TASK_COMPLETION_RAPORT'})
            smach.StateMachine.add('TASK_COMPLETION_RAPORT', TaskCompletionRaport(),
                                   transitions={'tr_complete_return_to_safe_state':
                                                'tr_return_to_save_state',
                                                'tr_suspended_return_to_manual':
                                                'tr_task_suspended'},
                                   remapping={'auto_task_monitor_input':
                                              'auto_task_monitor_output',
                                              'auto_task_blocked_input':
                                              'auto_task_blocked_output',
                                              'task_completion_man_out':
                                              'sm_auto_out_man',
                                              'task_completion_safe_out':
                                              'sm_auto_out_safe'})
         #Finish state machine transitions and add it to agribot_state_machine
        smach.StateMachine.add('AUTONOMOUS_CONTROL_STATE', sm_auto_ctrl,
                               transitions={'tr_task_configuration_failed':
                                            'MANUAL_CONTROL_STATE',
                                            'tr_task_suspended':
                                            'MANUAL_CONTROL_STATE',
                                            'tr_return_to_save_state':
                                            'SAFE_STATE'},
                               remapping={'sm_auto_in':
                                          'ag_auto_in',
                                          'sm_auto_out_man':
                                          'ag_manual_in',
                                          'sm_auto_out_safe':
                                          'ag_safe_in'})
    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('agribot_state_machine_intro_server',
                                        sm_agribot,
                                        '/SM_ROOT')
    sis.start()
    # Execute SMACH plan
    try:
        sm_agribot.execute()
    except smach.exceptions.InvalidUserCodeError as exc:
        # pylint: disable=superfluous-parens
        print("[State Machine] exception: " + str(exc))
    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()
