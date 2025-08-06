screen_start = 0
screen_main = 1
screen_config1 = 2
screen_config2 = 3
screen_devinfo = 4
screen_backlight = 5
screen_vibraset = 6
screen_freqset = 7
screen_pluse = 8
screen_duty = 9
screen_worktim = 10
screen_stimutime = 11
screen_ultraenable = 12
screen_workmodule = 13
screen_powerlevel= 14
screen_ultraen2 = 15
screen_record = 17

flash_len = 20
flash_light = 0
flash_vibra = 20
flash_freq = 40
flash_pluse = 60
flash_duty = 80
flash_worktim = 100
flash_stimutime = 120
flash_ultraenable = 140
flash_workmodule = 160
flash_powerlevel = 180
flash_ultralocation = 200

local screen_id = 0
local CmdSet = 0
local WorkTime = 1800
local lightlevel = 64
local WorkModule = 0
local PowerLevel = 1

local FlashSaveBuff = {}
local SceenCmdBuff = {[0]=0xEE,0xB5,0xB2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFC,0xFF,0xFF}

local record_buff = {[0] = "2025:12:1:16:56","2025:12:1:16:57"}
local record_index = 0

local UltraChEnableNum = 0
local UltraChEnableId = {[0] = 0,0,0,0,0,0,0,0,0}
--初始化函数
function on_init()
set_visiable(screen_main,6,0)
--set_visiable(screen_record,8,0)
DevParamInit()
start_timer(0,1000,0,0)
stop_timer(4)
screen_id = get_current_screen()
end

--定时回调函数，系统每隔1秒钟自动调用。
--function on_systick()
--end
CycleTime = 0
State = 0
--tim0 界面跳转
--tim1 显示剩余时间
--tim2 发送第二遍指令
--tim3 探头闪灯
--tim4 结束蜂鸣器
--定时器超时回调函数，当设置的定时器超时时，执行此回调函数，timer_id为对应的定时器ID
beep_count = 0
function on_timer(timer_id)
	
	StimuTime = get_value(screen_stimutime,1) / 10			--定时器以10为单位   闪灯时时间间隔除以10
	IdleTime  = get_value(screen_stimutime,2) / 10

	if timer_id == 0 then													--上电延迟进入主界面
		set_backlight(lightlevel)
		if screen_id== screen_start then
			change_screen(screen_main)
			stop_timer(0)
		end
	end

	if timer_id == 1 then
		if WorkTime > 0 then
			WorkTime = WorkTime - 1
			ShowTime(WorkTime)
		else	
			ShowRTCTime(0)
 			CmdSet = screen_main	
			record_index = record_index + 1
			local record_data = 'record_index' .. ';' .. record_buff[0] .. ';' .. record_buff[1] ..';'
			record_add(screen_record,1,record_data)
			set_text(screen_main,1,"治疗已完成")
 			set_enable(screen_main,2,1)	
 			set_value(screen_main,3,0)	
			set_value(screen_main,5,0)
			set_value(screen_main,6,0)
			set_value(screen_main,8,0)
			set_value(screen_main,9,0)	
			set_value(screen_main,7,0)
			set_value(screen_main,10,0)
			set_value(screen_main,11,0)
			set_value(screen_main,12,0)		
 			SendWorkStartStop()		
			stop_timer(1)
			stop_timer(3)	
 			start_timer(4,300,0,0)		
 			start_timer(2,100,0,0)	
		end
	end

	if timer_id == 2 then										--第二次发送指令
		if CmdSet == screen_main then
			SendWorkStartStop()
 		stop_timer(2)	
		elseif CmdSet == screen_vibraset then
			SendVibraParam()
 			stop_timer(2)	
		elseif CmdSet == screen_freqset then
			SendFreqParam()
 			stop_timer(2)	
		elseif CmdSet == screen_pluse then
			SendPluseParam()
 			stop_timer(2)	
		elseif CmdSet == screen_duty then
			SendDutyParam()
 			stop_timer(2)	
		elseif CmdSet == screen_stimutime then
			SendStiumParam()
 			stop_timer(2)	
		elseif CmdSet == screen_ultraenable then
			SendUltraEnable()
 			stop_timer(2)	
		elseif CmdSet == screen_workmodule then
			SendWorkModule()
 			CmdSet = screen_stimutime
		elseif CmdSet == screen_powerlevel then
			SendPowerLevel()
 			stop_timer(2)	
		end
		
	end

	if timer_id == 3 then
		CycleTime = CycleTime + 1
		if CycleTime <= StimuTime  and State == 0 then

			State = 1
		elseif (CycleTime >= StimuTime) and (CycleTime <= (IdleTime  + StimuTime) / 2) and State == 1 then

			State = 2
		elseif (CycleTime >= (IdleTime  + StimuTime) / 2) and (CycleTime <= (IdleTime  * 0.5 + StimuTime * 1.5)) and State == 2 then

			State = 3
		elseif (CycleTime >= (IdleTime  * 0.5 + StimuTime * 1.5)) and (CycleTime <= IdleTime + StimuTime) and State == 3 then

			State = 0
		elseif CycleTime > (IdleTime + StimuTime) then
			State = 0
			CycleTime = 0
		end
	end

	if timer_id == 4 then	
		beep_count = beep_count  + 1
		if (beep_count < 4) or (beep_count > 5) then
			beep(300)
		end

		if beep_count > 7 then
			beep_count = 0
			stop_timer(4)
		end
	end
end

--用户通过触摸修改控件后，执行此回调函数。
--点击按钮控件，修改文本控件、修改滑动条都会触发此事件。
function on_control_notify(screen,control,value)
	--设备工作开关
	if(screen == screen_main) and (control == 3) and (value == 1) then
		beep(150)
		WorkTime = get_value(screen_worktim,1) * 60
		ShowTime(WorkTime)
		set_text(screen_main,1,"剩余治疗时间")
		SendWorkStartStop()
		CmdSet = screen_main
		ShowRTCTime(1)
		start_timer(1,999,0,0)
		start_timer(3,5,0,0)	
		start_timer(2,100,0,0)	
 		stop_timer(4)	
	elseif(screen == screen_main) and (control == 3) and (value == 0) then
		WorkTime = 0
		CycleTime = 0
		ShowTime(WorkTime)
		ShowRTCTime(0)
		set_value(screen_main,5,0)
		set_value(screen_main,6,0)
		set_value(screen_main,8,0)
		set_value(screen_main,9,0)	
		set_value(screen_main,7,0)
		set_value(screen_main,10,0)
		set_value(screen_main,11,0)
		set_value(screen_main,12,0)	
		set_text(screen_main,1,"治疗已完成")
		SendWorkStartStop()
		
		local record_data = 'record_index' .. ';' .. record_buff[0] .. ';' .. record_buff[1] ..';'
		record_add(screen_record,1,record_data)
		CmdSet = screen_main

		stop_timer(1)
		stop_timer(3)
		start_timer(2,100,0,0)	
 		start_timer(4,300,0,0)			
	end

	--根据进度条设置屏幕亮度
	if (screen == screen_backlight) and (control == 1) then
		if value <= 5 then
			set_value(screen_backlight,1,5)
		end
		set_backlight(get_value(screen_backlight,1) * 0.8)
	end

	--保存背光参数到flash
	if (screen == screen_backlight) and (control == 2) and (value == 1) then
		FlashSaveLight()
	end

	--震动参数保存触发通知
	if (screen == screen_vibraset) and (control == 4) and (value == 1) then
		SendVibraParam()
		write_flash(flash_vibra,FlashSaveBuff)
		CmdSet = screen_vibraset
		start_timer(2,100,0,0)
	end

	--频率参数保存触发通知
	if (screen == screen_freqset) and (control == 9) and (value == 1) then
		SendFreqParam()
		write_flash(flash_freq,FlashSaveBuff)
		CmdSet = screen_freqset
		start_timer(2,100,0,0)
	end

	--脉冲周期参数保存触发通知
	if (screen == screen_pluse) and (control == 11) and (value == 1) then
		SendPluseParam()
		write_flash(flash_pluse,FlashSaveBuff)
		CmdSet = screen_pluse
		start_timer(2,100,0,0)
	end

	if screen == screen_pluse then
		if control == 1 or control == 2 or control == 3 or control == 4 or control == 5 or control == 6 or control == 7 or control == 8 then
			if value > get_value(screen_stimutime,1) then
				set_value(screen_pluse,control,get_value(screen_stimutime,1))
			end
		end
	end

	--脉冲占空比参数保存触发通知
	if (screen == screen_duty) and (control == 11) and (value == 1) then
		SendDutyParam()
		write_flash(flash_duty,FlashSaveBuff)
		CmdSet = screen_duty
		start_timer(2,100,0,0)
	end

	--工作时间参数保存触发通知
	if (screen == screen_worktim) and (control == 4) and (value == 1) then
		FlashSaveWorkTime()
	end

	--刺激时间参数保存触发通知
	local pluse_max = 0
	--设置刺激时长
	if screen == screen_stimutime and control == 1 then
		--检索最大的脉冲周期
		for i=1,8 do
			if pluse_max < get_value(screen_pluse,i) then
				pluse_max = get_value(screen_pluse,i)
			end
		end
		--如果刺激时长小于脉冲周期，将最大的脉冲周期赋值给刺激时长
		if value < pluse_max then
			value = pluse_max
			set_value(screen_stimutime,1,value)
		end

		if WorkModule == 1 then   --同步模式下不对间歇时长 进行限制
		elseif WorkModule == 2 and value > get_value(screen_stimutime,2) then --交替模式下间歇时长小于刺激时长，增加间歇时长
			set_value(screen_stimutime,2,get_value(screen_stimutime,1))
		elseif WorkModule == 3 and (value * 7 ) > get_value(screen_stimutime,2) then --循环模式
			set_value(screen_stimutime,2,get_value(screen_stimutime,1) * 7)
		elseif WorkModule == 4 and (value * 3 ) > get_value(screen_stimutime,2) then --交替模式2
			set_value(screen_stimutime,2,get_value(screen_stimutime,1) * 3)
		end
		set_value(screen_stimutime,3,WorkModule)
	end

	if screen == screen_stimutime and control == 2 then
		if WorkModule == 1 then   --同步模式下不对间歇时长 进行限制
		elseif WorkModule == 2 and value < get_value(screen_stimutime,1) then --交替模式下间歇时长小于刺激时长，增加间歇时长
			set_value(screen_stimutime,2,get_value(screen_stimutime,1))
		elseif WorkModule == 3 and value < (get_value(screen_stimutime,1) * 7) then --循环模式
			set_value(screen_stimutime,2,get_value(screen_stimutime,1) * 7)
		elseif WorkModule == 4 and value < (get_value(screen_stimutime,1) * 3) then --交替模式2
			set_value(screen_stimutime,2,get_value(screen_stimutime,1) * 3)
		end
	end
		
	if (screen == screen_stimutime) and (control == 6) and (value == 1) then
		SendStiumParam()
		write_flash(flash_stimutime,FlashSaveBuff)
		CmdSet = screen_stimutime
		start_timer(2,100,0,0)
	end

	--超声使能开关保存触发通知
	if (screen == screen_ultraenable) and (control == 19) and (value == 1) then
		SendUltraEnable()
		write_flash(flash_ultraenable,FlashSaveBuff)
		CmdSet = screen_ultraenable
		start_timer(2,100,0,0)
	end

	--超声使能开关全选触发通知
	if (screen == screen_ultraenable) and (control == 9) then
		if value == 1 then
			for i=1,8 do
				set_value(screen_ultraenable,i,1)
			end
		else
			for i=1,8 do
				set_value(screen_ultraenable,i,0)
			end
		end
	end

	--超声工作模式
	if(screen == screen_workmodule) and (control == 8) then
		if value == 1 then 
			if get_value(screen_workmodule,1) == 1 then
				WorkModule = 1
			elseif get_value(screen_workmodule,2) == 1 then
				WorkModule = 2
				if get_value(screen_stimutime,1) > get_value(screen_stimutime,2) then --交替模式下间歇时长小于刺激时长，增加间歇时长
					set_value(screen_stimutime,2,get_value(screen_stimutime,1))
				end
			elseif get_value(screen_workmodule,3)  == 1 then
				WorkModule = 3
 				if get_value(screen_stimutime,1)*7  > get_value(screen_stimutime,2) then --交替模式下间歇时长小于刺激时长，增加间歇时长
					set_value(screen_stimutime,2,get_value(screen_stimutime,1)*7)
				end	
			elseif 	get_value(screen_workmodule,4)  == 1 then
				WorkModule = 4
 				if get_value(screen_stimutime,1)*3  > get_value(screen_stimutime,2) then --交替模式下间歇时长小于刺激时长，增加间歇时长
					set_value(screen_stimutime,2,get_value(screen_stimutime,1)*3)
				end		
			else
				set_value(screen_workmodule,2,1)
 				WorkModule = 2	
			end
 			CmdSet = screen_workmodule	
 			SendWorkModule()	
 			write_flash(flash_workmodule,FlashSaveBuff)	
			start_timer(2,100,0,0)	
		end
	end

	--输出功率调整
	if (screen == screen_powerlevel) and (control == 5) then
		if value == 1 then
			PowerLevel = get_value(screen_powerlevel,3)
			CmdSet = screen_powerlevel
 			SendPowerLevel()	
			write_flash(flash_powerlevel,FlashSaveBuff)	
			start_timer(2,100,0,0)	
		end
	end

	--预设探头位置
	if screen == screen_ultraen2 then
		UltraChEnable(control,value)
	end

	if screen == screen_record then
		if control == 6 and value == 1 then
			record_clear(screen_record,1)				--清空日志
		end
	end
	return 1
end

--当画面切换时，执行此回调函数，screen为目标画面。
function on_screen_change(screen)
	screen_id = screen
end

--设备参数初始化
--读取flash内部数据
--对显示的参数进行初始化
function DevParamInit()
	local param_read = {}
	local param_read2 = {}
	
	--初始化主界面和工作时间界面参数
	param_read = read_flash(flash_worktim,flash_len)			
	if param_read[0] == 0xFF then
		WorkTime = 1800
		set_value(screen_worktim,1,WorkTime / 60)
		ShowTime(WorkTime)
	else
		WorkTime = (param_read[0] << 8) | param_read[1]
		set_value(screen_worktim,1,WorkTime / 60)
		ShowTime(WorkTime)
	end

	--初始化背光参数
	param_read = read_flash(flash_light,flash_len)				
	if (param_read[0] == 0xFF) or (param_read[0] == 0) then
		set_value(screen_backlight,1,80)
		set_backlight(80 * 0.8)
	else
		set_value(screen_backlight,1,param_read[0])
		lightlevel = get_value(screen_backlight,1) * 0.8
	end

	--初始化震动参数
	param_read = read_flash(flash_vibra,flash_len) 					
	if (param_read[0] == 0xFF) then
		set_value(screen_vibraset,1,1)							--震动开关
 		set_value(screen_vibraset,2,10)							--震动强度	
	else 
		set_value(screen_vibraset,1,param_read[0])
 		set_value(screen_vibraset,2,param_read[1])
	end

	--初始化频率参数
	param_read = read_flash(flash_freq,flash_len)     			
	if param_read[0] == 0xFF then
		for i=1,8 do
			set_value(screen_freqset,i,625)						--驱动频率默认值625KHz
		end
	else
		set_value(screen_freqset,1,(param_read[0] << 4) | ((param_read[1] & 0xF0) >> 4))
		set_value(screen_freqset,2,((param_read[1] & 0x0F) << 8) | param_read[2])
		set_value(screen_freqset,3,(param_read[3] << 4) | ((param_read[4] & 0xF0) >> 4))
		set_value(screen_freqset,4,((param_read[4] & 0x0F) << 8) | param_read[5])
		set_value(screen_freqset,5,(param_read[6] << 4) | ((param_read[7] & 0xF0) >> 4))
		set_value(screen_freqset,6,((param_read[7] & 0x0F) << 8) | param_read[8])
		set_value(screen_freqset,7,(param_read[9] << 4) | ((param_read[10] & 0xF0) >> 4))
		set_value(screen_freqset,8,((param_read[10] & 0x0F) << 8) | param_read[11])
	end

	--初始化脉冲周期参数
	param_read = read_flash(flash_pluse,flash_len)				
	if param_read[0] == 0xFF then
		for i=1,8 do
			set_value(screen_pluse,i,100)						--脉冲周期默认值100ms
			set_value(screen_pluse,i,100)
		end
	else
		set_value(screen_pluse,1,(param_read[0] << 4) | ((param_read[1] & 0xF0) >> 4))
		set_value(screen_pluse,2,((param_read[1] & 0x0F) << 8) | param_read[2])
		set_value(screen_pluse,3,(param_read[3] << 4) | ((param_read[4] & 0xF0) >> 4))
		set_value(screen_pluse,4,((param_read[4] & 0x0F) << 8) | param_read[5])
		set_value(screen_pluse,5,(param_read[6] << 4) | ((param_read[7] & 0xF0) >> 4))
		set_value(screen_pluse,6,((param_read[7] & 0x0F) << 8) | param_read[8])
		set_value(screen_pluse,7,(param_read[9] << 4) | ((param_read[10] & 0xF0) >> 4))
		set_value(screen_pluse,8,((param_read[10] & 0x0F) << 8) | param_read[11])
	end

	--初始化脉冲占空比参数
	param_read = read_flash(flash_duty,flash_len)				
	if (param_read[0] == 0xFF) or (param_read[0] == 0) then
		for i=1,8 do
			set_value(screen_duty,i,50)							--脉冲占空比默认值50%
		end
	else
		for i=1,8 do
			set_value(screen_duty,i,param_read[i-1])
		end
	end
	
	--初始化刺激时间参数
	param_read = read_flash(flash_stimutime,flash_len)			--初始化刺激时间参数
	if param_read[0] == 0xFF then
		set_value(screen_stimutime,1,1000)						--刺激时间默认值1000ms	
		set_value(screen_stimutime,2,1000)						--间歇时间默认值1000ms
	else
		set_value(screen_stimutime,1,param_read[0] << 8 | param_read[1])				
		set_value(screen_stimutime,2,param_read[2] << 8 | param_read[3])
	end

	--初始化超声使能参数
	param_read = read_flash(flash_ultraenable,flash_len)			--初始化超声使能参数
	if param_read[0] == 0xFF then
		for i=1,9 do
			set_value(screen_ultraenable,i,1)						--超声使能默认值关闭
		end
	else
		for i=1,9 do
			set_value(screen_ultraenable,i,param_read[i-1])
		end
	end

	--初始化工作模式
	param_read = read_flash(flash_workmodule,flash_len)
	if param_read[0] == 0xFF or param_read[0] == 0 then
 		set_value(screen_workmodule,2,1)
		WorkModule = 2
	else
		if param_read[0] == 1 then					--同步模式
			set_value(screen_workmodule,1,1)
		elseif param_read[0] == 2 then			 --交替模式1
 			set_value(screen_workmodule,2,1)
		elseif param_read[0] == 3 then			 --循环模式
			set_value(screen_workmodule,3,1)
		elseif param_read[0] == 4 then			--交替模式2
 			set_value(screen_workmodule,4,1)	
		end
	end

	--初始化输出功率 
	param_read = read_flash(flash_powerlevel,flash_len)
	if param_read[0] == 0xFF or param_read[0] == 0 then
		set_value(screen_powerlevel,3,1)
	else
		set_value(screen_powerlevel,3,param_read[0])
	end

	--初始化探头位置
	param_read = read_flash(flash_ultralocation,flash_len)
	for i = 1,28 do
		set_visiable(screen_main,i+9,0)
	end	

	if param_read[8] == 0xFF or param_read[8] == 0x00 then
	else
		UltraChEnableNum = param_read[8]
		for i=1,param_read[8] do
			set_value(screen_ultraen2,param_read[i-1],1)
 			set_visiable(screen_main,param_read[i-1]+8,1)	
 			UltraChEnableId[i-1] = param_read[i-1]
		end
	end	
end
--
--  串口接收函数
--
function on_uart_recv_data(packet)
	if packet[2] == 0x30 and packet[3] == 0x01 then 
		set_visiable(screen_main,6,1)
	elseif packet[2] == 0x30 and packet[3] == 0x00 then
		set_visiable(screen_main,6,0)
	end
end

--
--
--				指令发送函数
--
--

--设备启停指令
function SendWorkStartStop()
	SceenCmdBuff[2] = 0xB1
	SceenCmdBuff[3] = get_value(screen_main,3) & 0xFF
	uart_send_data(SceenCmdBuff)
end

--发送频率参数到下位机
function SendFreqParam()									
	SceenCmdBuff[2] = 0xB2
	SceenCmdBuff[3] = (get_value(screen_freqset,1) >> 4) & 0xFF
	SceenCmdBuff[4] = ((get_value(screen_freqset,1) << 4) & 0xF0) | ((get_value(screen_freqset,2) >> 8)  & 0x0F)
	SceenCmdBuff[5] = get_value(screen_freqset,2) & 0xFF
	SceenCmdBuff[6] = (get_value(screen_freqset,3) >> 4) & 0xFF
	SceenCmdBuff[7] = ((get_value(screen_freqset,3) << 4) & 0xF0) | ((get_value(screen_freqset,4) >> 8)  & 0x0F)
	SceenCmdBuff[8] = get_value(screen_freqset,4) & 0xFF
	SceenCmdBuff[9] = (get_value(screen_freqset,5) >> 4) & 0xFF
	SceenCmdBuff[10] = ((get_value(screen_freqset,5) << 4) & 0xF0) | ((get_value(screen_freqset,6) >> 8)  & 0x0F)
	SceenCmdBuff[11] = get_value(screen_freqset,6) & 0xFF
	SceenCmdBuff[12] = (get_value(screen_freqset,7) >> 4) & 0xFF
	SceenCmdBuff[13] = ((get_value(screen_freqset,7) << 4) & 0xF0) | ((get_value(screen_freqset,8) >> 8)  & 0x0F)
	SceenCmdBuff[14] = get_value(screen_freqset,8) & 0xFF

	for i=1,12 do
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送震动参数到下位机
function SendVibraParam()
	SceenCmdBuff[2] = 0xB3
	SceenCmdBuff[3] = get_value(screen_vibraset,1) & 0xFF
	SceenCmdBuff[4] = get_value(screen_vibraset,2) & 0xFF 

	FlashSaveBuff[0] = SceenCmdBuff[3]
	FlashSaveBuff[1] = SceenCmdBuff[4]

	uart_send_data(SceenCmdBuff)
end

--发送脉冲占空比参数到下位机
function SendDutyParam()
	SceenCmdBuff[2] = 0xB4
	for i=1,8 do
		SceenCmdBuff[3+i-1] = get_value(screen_duty,i) & 0xFF
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送脉冲周期参数到下位机
function SendPluseParam()
	SceenCmdBuff[2] = 0xB5
	SceenCmdBuff[3] = (get_value(screen_pluse,1) >> 4) & 0xFF
	SceenCmdBuff[4] = ((get_value(screen_pluse,1) << 4) & 0xF0) | ((get_value(screen_pluse,2) >> 8)  & 0x0F)
	SceenCmdBuff[5] = get_value(screen_pluse,2) & 0xFF
	SceenCmdBuff[6] = (get_value(screen_pluse,3) >> 4) & 0xFF
	SceenCmdBuff[7] = ((get_value(screen_pluse,3) << 4) & 0xF0) | ((get_value(screen_pluse,4) >> 8)  & 0x0F)
	SceenCmdBuff[8] = get_value(screen_pluse,4) & 0xFF
	SceenCmdBuff[9] = (get_value(screen_pluse,5) >> 4) & 0xFF
	SceenCmdBuff[10] = ((get_value(screen_pluse,5) << 4) & 0xF0) | ((get_value(screen_pluse,6) >> 8)  & 0x0F)
	SceenCmdBuff[11] = get_value(screen_pluse,6) & 0xFF
	SceenCmdBuff[12] = (get_value(screen_pluse,7) >> 4) & 0xFF
	SceenCmdBuff[13] = ((get_value(screen_pluse,7) << 4) & 0xF0) | ((get_value(screen_pluse,8) >> 8)  & 0x0F)
	SceenCmdBuff[14] = get_value(screen_pluse,8) & 0xFF

	for i=1,12 do
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送刺激时间参数到下位机
function SendStiumParam()
	SceenCmdBuff[2] = 0xB6
	SceenCmdBuff[3] = (get_value(screen_stimutime,1) >> 8) & 0xFF
	SceenCmdBuff[4] = get_value(screen_stimutime,1) & 0xFF 
	SceenCmdBuff[5] = (get_value(screen_stimutime,2) >> 8) & 0xFF
	SceenCmdBuff[6] = get_value(screen_stimutime,2) & 0xFF 

	for i=1,4 do
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送超声通道使能到下位机
function SendUltraEnable()
	SceenCmdBuff[2] = 0xB7
	for i=1,9 do
		SceenCmdBuff[3+i-1] = get_value(screen_ultraenable,i) & 0xFF
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送超声通道使能到下位机
function SendUltraEnable()
	SceenCmdBuff[2] = 0xB7
	for i=1,9 do
		SceenCmdBuff[3+i-1] = get_value(screen_ultraenable,i) & 0xFF
		FlashSaveBuff[i-1] = SceenCmdBuff[3+i-1]
	end

	uart_send_data(SceenCmdBuff)
end

--发送超声工作模式到下位机
function SendWorkModule()
	SceenCmdBuff[2] = 0xB8
	SceenCmdBuff[3] = WorkModule
	
    FlashSaveBuff[0] = 	WorkModule
	uart_send_data(SceenCmdBuff)
end

--发送超声功率到下位机
function SendPowerLevel()
	SceenCmdBuff[2] = 0xB9
	SceenCmdBuff[3] = PowerLevel - 1
	
    FlashSaveBuff[0] = 	PowerLevel
	uart_send_data(SceenCmdBuff)
end

--保存背光参数到flash
function FlashSaveLight()
	FlashSaveBuff[0] = get_value(screen_backlight,1)
	write_flash(flash_light,FlashSaveBuff)
end

--保存工作时长参数到flash
function FlashSaveWorkTime()
	local WorkTime = get_value(screen_worktim,1) * 60
	FlashSaveBuff[0] = (WorkTime >> 8) & 0xFF
	FlashSaveBuff[1] = WorkTime & 0xFF
	WorkTime = (FlashSaveBuff[0] << 8) | FlashSaveBuff[1]
	ShowTime(WorkTime)
	write_flash(flash_worktim,FlashSaveBuff)
end

local UltraChEnbaleFlg = {[0]=0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
function UltraChEnable(control,value)

	UltraChEnableId[8] = UltraChEnableNum
	if control == 32 and value == 1 then
		write_flash(flash_ultralocation,UltraChEnableId)
		for i=1,28 do
			set_visiable(screen_main,i+9,0)
		end
		for i=1,UltraChEnableNum do
			set_visiable(screen_main,UltraChEnableId[i-1] + 8,1)
		end
	elseif control > 1 and control < 30 then 
		if value == 1 then
			UltraChEnableId[UltraChEnableNum] = control
			UltraChEnableNum = UltraChEnableNum + 1
			UltraChEnbaleFlg[control - 2] = 1
		else
			for i=1,8 do
				if UltraChEnableId[i-1] == control then
					for j=i,UltraChEnableNum do
						UltraChEnableId[j-1] = UltraChEnableId[j]
					end
					break
				end
			end
			UltraChEnableNum = UltraChEnableNum - 1	
			UltraChEnableId[UltraChEnableNum] = 0
	 		UltraChEnbaleFlg[control - 2] = 0	
		end

		if UltraChEnableNum == 8 then
			for i=1,29 do
				if UltraChEnbaleFlg[i - 1] == 0 then
					set_enable(screen_ultraen2, i+1,0)
				end
	       end
	    elseif UltraChEnableNum == 7 then
			for i=1,29 do
				set_enable(screen_ultraen2, i+1,1)
			end
		end
	end
end

--显示主界面时间
function ShowTime(work_tim)
	local min = work_tim / 60
	local sec = work_tim % 60

	local cmin = tostring(math.floor(min))			--整形转字符串
	local csec = tostring(math.floor(sec))			

	if #cmin == 1 then
		cmin = "0" .. cmin
	end

	if #csec == 1 then
		csec = "0" .. csec
	end
	local time_str = cmin .. ":" .. csec

	set_text(screen_main,4,time_str)
end

--日志操作记录 记录使用次数 开始时间 结束时间
function ShowRTCTime(work_sta)
	local year
	local tim_buff = {[0] = "01","01","00","00","0","0"}
	year, tim_buff[0], tim_buff[1], tim_buff[2], tim_buff[3], tim_buff[4], tim_buff[5] = get_date_time()

	for i = 1, 5 do
		if tim_buff[i-1] < 10 then
			tim_buff[i-1] = "0" .. tim_buff[i-1]
		end
	end

	if work_sta == 1 then
		record_buff[0] = year .. "-" .. tim_buff[0] .. "-" .. tim_buff[1] .. " " .. tim_buff[2] .. ":" .. tim_buff[3] .. ":" .. tim_buff[4]
	elseif work_sta == 0 then
		record_buff[1] = year .. "-" .. tim_buff[0] .. "-" .. tim_buff[1] .. " " .. tim_buff[2] .. ":" .. tim_buff[3] .. ":" .. tim_buff[4]
	end
end

