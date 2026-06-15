import gfx.managers.FocusHandler;
import gfx.io.GameDelegate;
import gfx.ui.NavigationCode;

import mx.utils.Delegate;

import FTA.Utility;
import FTA.Defines.KeyCode;


class SleepWaitMenu extends MovieClip
{
	// From the old UI
	public var HoursSlider: MovieClip;

	public var HoursText: TextField;
	public var CurrentTime: TextField;
	public var QuestionInstance: TextField;

	public var bDisableControls: Boolean = false;

	// General
	private var confirm_btn: MovieClip;
	private var cancel_btn: MovieClip;
	private var switch_btn: MovieClip;
	private var CurrentDate: MovieClip;
	private var iconWeather: MovieClip;
	private var container: MovieClip;
	private var bar: MovieClip;
	private var hitbox: MovieClip;
	private var progressBar: MovieClip;
	private var cursor: MovieClip;
	private var pointer_mc: MovieClip;
	private var outerRing_mc: MovieClip;

	private var NewTime: TextField;
	private var CurTime: TextField;
	private var WaitFormat: TextField;
	private var WaitTime: TextField;

	private var minute: String = "";

	private var currentHour: Number = 0;
	private var iPlatform: Number = 0;

	private var isWaiting: Boolean = false;
	private var isWaitingDays: Boolean = false;

	// Set by DLL
	public var is24Clock: Boolean = false;
	public var useLeadingZero: Boolean = true;
	public var suffixAM: String = "";
	public var suffixPM: String = "";
	public var isVR: Boolean = false;
	public var useCustomCursor: Boolean = true;
	// ----------

	// onMouseMove()
	private var bInArea: Boolean = false;

	// changeWaitTime()
	private var iOldClickHour: Number = 0;
	
	// setTimeAndDate()
	private var bDoOnce: Boolean = false;

	// Controlled by getter and setter functions
	private var _disableControls: Boolean = false;
	private var _waitHours: Number = 1;


	public function SleepWaitMenu()
	{
		super();
	}

	public function InitExtensions(): Void
	{
		Mouse.addListener(this);
		FocusHandler.instance.setFocus(this, 0);

		HoursSlider.focusEnabled = false;

		confirm_btn.addEventListener("click", this, "onOKPress");
		cancel_btn.addEventListener("click", this, "onCancelPress");
		switch_btn.addEventListener("click", this, "onSwitch");
	
		hitbox.onRollOver = hitbox.onRollOut = Delegate.create(this, handleRollActions);
		hitbox.onRelease = Delegate.create(this, onBarRelease);

		container = progressBar.bar.mask;
		progressBar.bar.fill.setMask(container);
	}

	public function SetPlatform(aiPlatformIndex: Number, abPS3Switch: Boolean): Void
	{
		iPlatform = aiPlatformIndex;

		confirm_btn.SetPlatform(aiPlatformIndex, abPS3Switch);
		cancel_btn.SetPlatform(aiPlatformIndex, abPS3Switch);
		switch_btn.SetPlatform(aiPlatformIndex, abPS3Switch);

		var halfStage: Number = Stage.width / 2;
		confirm_btn._x = halfStage - confirm_btn._width - 5;
		cancel_btn._x = halfStage + 5;
		switch_btn._x = halfStage - (switch_btn._width / 2);
	}


	public function set disableControls(abFlag: Boolean): Void
	{
		_disableControls = bDisableControls = abFlag;
		HoursSlider.thumb.disabled = HoursSlider.track.disabled = confirm_btn.disabled = switch_btn.disabled = abFlag;
	}

	public function get disableControls(): Boolean
	{
		return _disableControls;
	}

	public function set waitHours(value: Number): Void
	{
		_waitHours = Utility.clamp(value, (isWaiting ? 0 : 1), isWaitingDays ? (isWaiting ? 768 : 32) : 24);
		updateWaitTime();
	}

	public function get waitHours(): Number
	{
		return _waitHours;
	}


	private function handleInput(details: gfx.ui.InputDetails, pathToFocus: Array): Boolean
	{
		var handledInput: Boolean = false;
		if (!disableControls && pathToFocus != undefined && pathToFocus.length > 0)
			handledInput = pathToFocus[0].handleInput(details, pathToFocus.slice(1));

		if (!handledInput && Shared.GlobalFunc.IsKeyPressed(details)) 
		{
			switch(details.code)
			{
				case KeyCode.C:
					onSwitch();
					break;
			}

			switch(details.navEquivalent)
			{
				case NavigationCode.TAB:
					onCancelPress();
					break;
				case NavigationCode.ENTER:
					onOKPress();
					break;
				case NavigationCode.LEFT:
					if (iPlatform == 0)
						onDPadInput(false, false, true, false);
					break;
				case NavigationCode.RIGHT:
					if (iPlatform == 0)
						onDPadInput(false, false, false, true);
					break;
				case NavigationCode.PAGE_UP:
				case NavigationCode.GAMEPAD_R1:
					onMouseWheel(4)
					break;
				case NavigationCode.PAGE_DOWN:
				case NavigationCode.GAMEPAD_L1:
					onMouseWheel(-4)
					break;
				case NavigationCode.GAMEPAD_X:
					onSwitch();
					break;
			}
		}

		return true;
	}

	private function onMouseMove(): Void
	{
		if (bInArea)
		{
			cursor._x = _xmouse;
			cursor._y = _ymouse;
		}

		if (!isWaiting && bInArea)
		{
			var barCenterX: Number = progressBar._x;
			var barCenterY: Number = progressBar._y;
			var angle: Number = Math.atan2(_ymouse - barCenterY, _xmouse - barCenterX) * 180 / Math.PI;

			var normalizedAngle: Number = (angle - (isWaitingDays ? 270 : 90)) % 360;
			if (normalizedAngle < 0)
				normalizedAngle += 360;

			var maxUnits: Number = isWaitingDays ? 32 : 24;

			var clickHour: Number = Math.round((normalizedAngle * maxUnits) / 360);
			if (clickHour <= 0)
				clickHour = maxUnits;
			
			changeWaitTime(clickHour);
		}
	}

	private function onMouseWheel(aiWheelVal: Number): Void
	{
		if (!disableControls)
		{
			waitHours += aiWheelVal;
			modifySliderValue(aiWheelVal);
		}
	}

	// DLL Call
	public function onStickLeft(afX: Number, afY: Number): Void
	{
		var magnitude: Number = Math.sqrt(afX * afX + afY * afY);
		// Only allow input when the stick is almost tilted to the max
		if (magnitude < 0.9)
			return;

		if (!isWaiting && !disableControls)
		{
			var angleRad: Number = Math.atan2(-afY, afX);
			var angleDeg: Number = angleRad * 180 / Math.PI;

			if (angleDeg < 0)
				angleDeg += 360;

			var normalizedAngle: Number = (angleDeg + (isWaitingDays ? 90 : 270)) % 360;
			var maxUnits: Number = isWaitingDays ? 32 : 24;

			var clickHour: Number = Math.round((normalizedAngle * maxUnits) / 360);
			if (clickHour <= 0)
				clickHour = maxUnits;

			changeWaitTime(clickHour);
		}
	}

	// DLL Call
	public function onDPadInput(up: Boolean, down: Boolean, left: Boolean, right: Boolean): Void
	{
		if (disableControls) return;

		var delta: Number = 0;
		if (left) delta -= 1;
		if (right) delta += 1;

		if (delta == 0) return;

		waitHours += delta;
		modifySliderValue(delta);
	}

	private function updateWaitTime(): Void
	{
		var waitNumber = isWaiting && isWaitingDays ? Math.ceil(waitHours / 24) : waitHours;
		WaitTime.SetText(waitNumber);

		GameDelegate.call("PlaySound", ["UIMenuPrevNext"]);

		if (!isWaiting)
		{
			drawMask((isWaitingDays ? 32 : currentHour), waitHours);
			setWaitTime();

			var iTimeUse: Number = currentHour + waitHours;
			if (iTimeUse > (isWaitingDays ? 32 : 24))
				iTimeUse -= (isWaitingDays ? 32 : 24);
		}

		if (isWaiting && isWaitingDays)
			progressBar.pinDay_mc.gotoAndStop(Math.ceil(waitHours / 24));
		else if (!isWaiting)
			progressBar[isWaitingDays ? "pinDay_mc" : "pinHour_mc"].gotoAndStop(isWaitingDays ? waitHours : iTimeUse);
	}

	private function onOKPress(event: Object): Void
	{
		if (disableControls)
			return;

		showCursor(true);

		isWaiting = disableControls = true;

		// Now we need to convert the day format into hours
		var newWaitTime: Number = (isWaitingDays ? 24 : 1) * waitHours;
		waitHours = newWaitTime
		HoursSlider.value = newWaitTime;

		GameDelegate.call("OK", [waitHours]);
	}

	private function onCancelPress(event: Object): Void
	{
		isWaiting = false;
		GameDelegate.call("Cancel", []);
	}

	private function onSwitch(event: Object): Void
	{
		if (disableControls)
			return;
		
		isWaitingDays = !isWaitingDays;

		progressBar.pinDay_mc._alpha = isWaitingDays ? 100 : 0;
		progressBar.pinHour_mc._alpha = isWaitingDays ? 0 : 100;

		waitHours = 1;

		WaitFormat.SetText(isWaitingDays ? "$fta_DAY" : "$fta_HOUR");
		switch_btn.label = isWaitingDays ? "$fta_SWITCH_HOURS" : "$fta_SWITCH_DAY";
		switch_btn.text = switch_btn.label;

		NewTime._alpha = pointer_mc._alpha = outerRing_mc._alpha = isWaitingDays ? 0 : 100;
		CurTime._x = isWaitingDays ? (Stage.width / 2) - (CurTime._width / 2) : (Stage.width / 2 - CurTime._width - 13);
		CurTime.autoSize = (isWaitingDays ? "center" : "right");
	}

	private function handleRollActions(): Void
	{
		if (!isWaiting)
		{
			bInArea = !bInArea;
			showCursor(!bInArea);
		}
	}

	private function onBarRelease(): Void
	{
		onOKPress();
		bInArea = false;
		showCursor(true);
	}


	/**
	 * DLL RECIEVER
	 * It sets the date and time for the menu
	 * 
	 * When not waiting, we update the slider, this normally only happens once on menu open
	 * 
	 * If waiting, we are redrawing the bar everytime this function gets called, this update
	 * happens every hour.
	*/
	public function setTimeAndDate(hours12: Number, hours24: Number, minute: String, dateString: String): Void
	{
		var sameHour: Boolean = currentHour == hours24;
		currentHour = hours24;

		if (isWaiting)
		{
			waitHours--;
			drawMask((isWaitingDays ? 32 : currentHour), (isWaitingDays ? Math.ceil(waitHours / 24) : waitHours));
		}
		else
		{
			this.minute = minute;

			// Suport for unpaused menu mods
			if (!sameHour && waitHours > 1)
			{
				waitHours -= 1;
				modifySliderValue(-1);
			}

			updateWaitTime();
		}

		setTimeText(hours12, dateString);
	}

	/**
	 * DLL RECIEVER
	 * This is for VR compatibility
	 * 
	 * On menu open this will be called by the DLL with a boolean to determine if the game
	 * is VR or not.
	 * If it is or not, additional actions can be done in here.
	*/
	public function setVR(isVR: Boolean): Void
	{
		if (!isVR)
			Stage.scaleMode = "showAll";
	}

	/**
	 * DLL RECIEVER
	 * Sets the correct weather icon
	*/
	public function updateWeather(classification: Number): Void
	{
		iconWeather._alpha = 100;
		iconWeather.gotoAndStop(classification + 1);
	}

	// API
	public function SetSleeping(aSleeping: Boolean): Void
	{
		QuestionInstance.SetText(aSleeping ? "$fta_REST" : "$fta_WAIT");
	}

	private function changeWaitTime(clickHour: Number): Void
	{
		if (iOldClickHour != clickHour)
		{
			iOldClickHour = clickHour;

			var maxUnits: Number = isWaitingDays ? 32 : 24;
			var startPoint: Number = isWaitingDays ? 32 : currentHour;

			var hoursDifference: Number = clickHour - startPoint;
			if (hoursDifference <= 0)
				hoursDifference += maxUnits;

			waitHours = hoursDifference;
			HoursSlider.value = hoursDifference;
		}
	}

	private function setTimeText(hours12: Number, dateString: String): Void
	{
		var timeString: String = "";
		if (is24Clock)
		{
			var leadingZero: String = (useLeadingZero && currentHour < 10) ? "0" : "";
			timeString = leadingZero + currentHour + ":" + minute;
		}
		else
		{
			var suffix: String = (currentHour < 12) ? suffixAM : suffixPM;
			var leadingZero: String = (useLeadingZero && (hours12 > 0 && hours12 < 10)) ? "0" : "";

			timeString = leadingZero + hours12 + ":" + minute + " " + suffix;
		}

		CurTime.SetText(timeString);
		CurrentDate.SetText(dateString, true);
	}

	private function setWaitTime(): Void
	{
		var newHour: Number = currentHour + waitHours;
		if (newHour >= 24)
			newHour -= 24;

		var sNewTime: String = "";
		if (is24Clock)
		{
			var leadingZero: String = (useLeadingZero && newHour < 10) ? "0" : "";
			sNewTime = leadingZero + newHour + ":" + minute;
		}
		else
			sNewTime = convertTo12HourFormat(newHour) + ":" + minute + " " + ((newHour < 12) ? suffixAM : suffixPM);

		NewTime.SetText(sNewTime);
	}

	private function showCursor(bShow: Boolean): Void
	{
		if (!useCustomCursor)
			return;

		Mouse[bShow ? "show" : "hide"]();
		// Following triggers an event inside the DLL
		skse.SendModEvent("MWMShowMouseCursor", "", bShow ? 1 : 0);
		cursor._alpha = bShow ? 0 : 100;
	}

	private function drawMask(currentTime: Number, waitTime: Number): Void
	{
		var centerX: Number = 0;
		var centerY: Number = 0;
		var radius: Number = 350;
		var fraction: Number = (360 / (isWaitingDays ? 32 : 24));

		var startAngle: Number = (isWaitingDays ? -90 : -270) + (currentTime * fraction);
		var endAngle: Number = startAngle + (waitTime * fraction);
		var angleStep: Number = 5;

		container.clear();
		container.beginFill(0xFFFFFF, 100);
		container.moveTo(centerX, centerY);

		for (var angle: Number = startAngle; angle < endAngle; angle += angleStep)
		{
			var radians: Number = (angle * Math.PI) / 180;
			container.lineTo(centerX + (radius * Math.cos(radians)), centerY + (radius * Math.sin(radians)));
		}

		var endRadians: Number = (endAngle * Math.PI) / 180;
		container.lineTo(centerX + (radius * Math.cos(endRadians)), centerY + (radius * Math.sin(endRadians)));

		container.lineTo(centerX, centerY);
		container.endFill();
	}

	private function convertTo12HourFormat(hour: Number): String
	{
		var hour12: Number = hour % 12;
		return (useLeadingZero && (hour12 > 0 && hour12 < 10) ? "0" : "") + (hour12 == 0 ? "12" : hour12.toString());
	}


	// No longer used but kept as the Engine is accessing them
	
	// API
	public function SetCurrentTime(aTime: String): Void
	{

	}

	// Bethesda made this work like hell.
	private function sliderChange(event: Object): Void
	{

	}

	// API
	// Bethesda made this work like hell.
	// Better not touch this anymore, this causes extrem carnage
	public function getSliderValue(): Number
	{
		return Math.floor(HoursSlider.value);
	}

	public function modifySliderValue(aiDelta: Number): Void
	{
		HoursSlider.value += aiDelta;
		sliderChange();
	}
}