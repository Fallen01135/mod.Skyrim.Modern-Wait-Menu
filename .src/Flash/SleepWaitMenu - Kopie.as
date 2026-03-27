import gfx.managers.FocusHandler;
import gfx.io.GameDelegate;
import gfx.ui.NavigationCode;

class SleepWaitMenu extends MovieClip
{
	var ButtonRect: MovieClip;
	var CurrentDate: MovieClip;
	var HoursSlider: MovieClip;
	var progressBar: MovieClip;
	var bar: MovieClip;
	var container: MovieClip;
	var iconWeather: MovieClip;
	var barBackground: MovieClip;
	var cursor: MovieClip;
	var CurrentTime: TextField;
	var HoursText: TextField;
	var QuestionInstance: TextField;
	var NewTime: TextField;

	var hourMinute: Array;
	var iHour: Number;
	var hours: Number = 1;
	var iCurStartHour: Number = 0;
	var iOldClickHour: Number = 0;

	var bDisableControls: Boolean;
	var bWaiting: Boolean = false;
	var bDoOnce: Boolean = false;
	var bInArea: Boolean = false;

	function SleepWaitMenu()
	{
		super();

		iconWeather._alpha = 0;
		bDisableControls = false;
	}

	function InitExtensions(): Void
	{
		skse.SendModEvent("TES4WaitMenu_UpdateWeather");

		Mouse.addListener(this);
		FocusHandler.instance.setFocus(HoursSlider, 0);

		ButtonRect.AcceptMouseButton.SetPlatform(0, false);
		ButtonRect.CancelMouseButton.SetPlatform(0, false);

		HoursSlider.addEventListener("change", this, "sliderChange");
		HoursSlider.scrollWheel = function ()
		{
		};

		ButtonRect.AcceptMouseButton.addEventListener("click", this, "onOKPress");
		ButtonRect.CancelMouseButton.addEventListener("click", this, "onCancelPress");
		barBackground.onRollOver = barBackground.onRollOut = handleRollActions;
		barBackground.onRelease = onBarRelease;

		container = progressBar.bar.createEmptyMovieClip("bar", this.getNextHighestDepth());
	}

	private function handleRollActions(): Void
	{
		if (!_parent.bWaiting)
		{
			_parent.bInArea = !_parent.bInArea;
			_parent.showCursor(!_parent.bInArea);
		}
	}

	private function onBarRelease(): Void
	{
		_parent.onOKPress();
		_parent.bInArea = false;
		_parent.showCursor(true);
	}

	private function showCursor(bShow: Boolean): Void
	{
		_root.Mouse[bShow ? "show" : "hide"]();
		skse.SendModEvent("TES4WaitMenu_ShowCursor", "", bShow ? 1 : 0);
		cursor._alpha = bShow ? 0 : 100;
		if (!bShow)
			cursor._x = cursor._y = -250
	}

	function handleInput(details: gfx.ui.InputDetails, pathToFocus: Array): Boolean
	{
		var handledInput: Boolean = false;
		if (!disableControls && pathToFocus != undefined && pathToFocus.length > 0)
			handledInput = pathToFocus[0].handleInput(details, pathToFocus.slice(1));

		if (!handledInput && Shared.GlobalFunc.IsKeyPressed(details)) 
		{
			switch(details.navEquivalent)
			{
				case NavigationCode.TAB:
					onCancelPress();
					break;
				case NavigationCode.ENTER:
					onOKPress();
					break;
				case NavigationCode.PAGE_UP:
				case NavigationCode.GAMEPAD_R1:
					if (!disableControls)
						modifySliderValue(4);
					break;
				case NavigationCode.PAGE_DOWN:
				case NavigationCode.GAMEPAD_L1:
					if (!disableControls)
						modifySliderValue(-4);
					break;
			}
		}

		return true;
	}

	function get disableControls(): Boolean
	{
		return bDisableControls;
	}

	function set disableControls(abFlag: Boolean): Void
	{
		bDisableControls = abFlag;
		HoursSlider.thumb.disabled = HoursSlider.track.disabled = ButtonRect.AcceptMouseButton.disabled = abFlag;
	}

	function modifySliderValue(aiDelta: Number): Void
	{
		HoursSlider.value += aiDelta;
		sliderChange();
	}

	function onMouseWheel(aiWheelVal: Number): Void
	{
		if (!disableControls)
		{
			HoursSlider.value += aiWheelVal;
			sliderChange();
		}
	}

	function sliderChange(event: Object): Void
	{
		hours = Math.floor(HoursSlider.value);
		HoursText.text = hours.toString();

		GameDelegate.call("PlaySound", ["UIMenuPrevNext"]);

		var iTimeUse: Number = iHour + hours;
		if (iTimeUse > 24)
			iTimeUse -= 24;

		drawBar(iHour, hours)

		if (!bWaiting)
		{
			setWaitTime();
			progressBar.gotoAndStop(iTimeUse);
		}
		else
			skse.SendModEvent("TES4WaitMenu_UpdateWeather");
	}

	function setWaitTime(): Void
	{
		var newHour: Number = iHour + hours;
		if (newHour > 24)
			newHour -= 24;

		newHour -= 1;

		var sNewTime: String = convertTo12HourFormat(newHour) + ":" + hourMinute[1] + " " + (newHour < 12 ? "AM" : "PM");

		NewTime.text = sNewTime;
	}

	function convertTo12HourFormat(hour: Number): String
	{
		var hour12: Number = hour % 12;
		return hour12 === 0 ? "12" : hour12.toString();
	}

	function updateWeather(): Void
	{
		var iWeatherClassification: Number = Number(arguments);
		iconWeather._alpha = iWeatherClassification == -1 ? 0 : 100;

		if (iWeatherClassification != -1)
			iconWeather.gotoAndStop(iWeatherClassification + 1);
	}

	function drawBar(iStartHour: Number, iNextHour: Number): Void
	{
		// Define the parameters for the circle
		var centerX: Number = 239.85; // X coordinate of the center
		var centerY: Number = 239.85; // Y coordinate of the center
		var radius: Number = 250; // Radius of the circle
		var fraction: Number = 15;
		var startAngle: Number = -270 + (bWaiting ? iStartHour : (iStartHour - 1)) * fraction; // Start angle (top of the circle)
		var endAngle: Number = startAngle + iNextHour * fraction; // End angle
		var angleStep: Number = 5; // Angle step for drawing the arc

		container.clear();
		container.beginFill(0x828282, 100);
		container.moveTo(centerX, centerY); // Move to the center
		for (var angle: Number = startAngle; angle <= endAngle; angle += angleStep)
		{
			var radians: Number = angle * Math.PI / 180;
			container.lineTo(centerX + radius * Math.cos(radians), centerY + radius * Math.sin(radians));
		}

		container.lineTo(centerX, centerY); // Draw a line back to the center to close the shape
		container.endFill();
		container.setMask(progressBar.bar.mask); // 'maskLayer' is the instance name of your mask layer

		iCurStartHour = iStartHour;
	}

	function getSliderValue(): Number
	{
		return Math.floor(HoursSlider.value);
	}

	function onOKPress(event: Object): Void
	{
		if (!disableControls)
		{
			showCursor(true);

			bWaiting = disableControls = true;
			GameDelegate.call("OK", [getSliderValue()]);
		}
	}

	function onCancelPress(event: Object): Void
	{
		bWaiting = false;
		GameDelegate.call("Cancel", []);
	}

	function SetCurrentTime(aTime: String): Void
	{
		var parts: Array = aTime.split(", ");

		// Convert 12 hour clock to 24 hour clock
		var splitTime: Array = parts[1].split(" ");
		hourMinute = splitTime[0].split(":");
		var iCurHour: Number = Number(hourMinute[0]);
		var period: String = splitTime[1];

		if (period == "PM" && iCurHour < 12)
			iCurHour += 12;
		else if (period == "AM" && iCurHour == 12)
			iCurHour = 0;
		// Converter Finished

		iHour = iCurHour + 1;

		if (!bWaiting && !bDoOnce)
		{
			setWaitTime();

			bDoOnce = true;
			progressBar.gotoAndStop(iHour + 1);
			drawBar(iHour, 1);
		}

		CurrentTime.SetText(parts[1]);
		CurrentDate.SetText(parts[0] + ", " + parts[2]);
	}

	function onMouseMove(): Void
	{
		if (bInArea)
		{
			cursor._x = _xmouse;
			cursor._y = _ymouse;
		}

		if (!bWaiting && bInArea)
		{
			var centerX: Number = Stage.width / 2; // X coordinate of the center
			var centerY: Number = Stage.height / 2; // Y coordinate of the center
			var angle: Number = Math.atan2(_ymouse - centerY, _xmouse - centerX) * 180 / Math.PI;
			var clickHour: Number = Math.round(((angle + 90) % 360) * 24 / 360);

			if (clickHour <= 0)
				clickHour += 24; // Ensure click hour is within the range of 1 to 24
			
			if (iOldClickHour != clickHour)
			{
				iOldClickHour = clickHour;

				// The following needs to be here two times
				var hoursDifference: Number = (clickHour - iCurStartHour) - 11;
				if (hoursDifference <= 0) hoursDifference += 24;
				if (hoursDifference <= 0) hoursDifference += 24;
				
				HoursSlider.value = hoursDifference;
				sliderChange();
			}
		}
	}

	function SetSleeping(aSleeping: Boolean): Void
	{
		var waitText: String = aSleeping ? "$TES4_REST" : "$TES4_WAIT";
		QuestionInstance.SetText(waitText);
	}

	function SetPlatform(aiPlatformIndex: Number, abPS3Switch: Boolean): Void
	{
		ButtonRect.AcceptGamepadButton._visible = aiPlatformIndex != 0;
		ButtonRect.CancelGamepadButton._visible = aiPlatformIndex != 0;
		ButtonRect.AcceptMouseButton._visible = aiPlatformIndex == 0;
		ButtonRect.CancelMouseButton._visible = aiPlatformIndex == 0;

		if (aiPlatformIndex != 0)
		{
			ButtonRect.AcceptGamepadButton.SetPlatform(aiPlatformIndex, abPS3Switch);
			ButtonRect.CancelGamepadButton.SetPlatform(aiPlatformIndex, abPS3Switch);
		}
	}
}