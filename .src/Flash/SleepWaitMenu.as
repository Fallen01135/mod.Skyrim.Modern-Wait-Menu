import gfx.managers.FocusHandler;
import gfx.io.GameDelegate;
import gfx.ui.NavigationCode;

import mx.utils.Delegate;


class SleepWaitMenu extends MovieClip
{
	public var HoursSlider: MovieClip;
	public var HoursText: TextField;
	public var CurrentTime: TextField;

	private var ButtonRect: MovieClip;
	private var CurrentDate: MovieClip;
	private var iconWeather: MovieClip;
	private var container: MovieClip;
	private var bar: MovieClip;
	private var barBackground: MovieClip;
	private var progressBar: MovieClip;
	private var cursor: MovieClip;

	private var NewTime: TextField;
	private var CurTime: TextField;
	private var WaitFormat: TextField;
	private var WaitTime: TextField;
	private var QuestionInstance: TextField;

	private var minute: Number = 0;
	private var iHour: Number = 0;
	private var hours: Number = 1;
	private var iOldClickHour: Number = 0;

	private var bWaiting: Boolean = false;
	private var bDoOnce: Boolean = false;
	private var bInArea: Boolean = false;

	// Set by DLL
	public var is24Clock: Boolean = false;
	public var useLeadingZero: Boolean = true;
	public var suffixAM: String = "";
	public var suffixPM: String = "";
	// ----------

	private var _disableControls: Boolean = false;


	public function SleepWaitMenu()
	{
		super();
	}

	public function InitExtensions(): Void
	{
		Stage.scaleMode = "showAll";

		Mouse.addListener(this);
		FocusHandler.instance.setFocus(this, 0);

		ButtonRect.AcceptMouseButton.SetPlatform(0, false);
		ButtonRect.CancelMouseButton.SetPlatform(0, false);

		HoursSlider.addEventListener("change", this, "sliderChange");
		HoursSlider.scrollWheel = function ()
		{
		};

		ButtonRect.AcceptMouseButton.addEventListener("click", this, "onOKPress");
		ButtonRect.CancelMouseButton.addEventListener("click", this, "onCancelPress");
		barBackground.onRollOver = barBackground.onRollOut = Delegate.create(this, handleRollActions);
		barBackground.onRelease = Delegate.create(this, onBarRelease);

		container = progressBar.bar.createEmptyMovieClip("bar", this.getNextHighestDepth());
		container.setMask(progressBar.bar.mask);
	}

	public function SetPlatform(aiPlatformIndex: Number, abPS3Switch: Boolean): Void
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


	public function set disableControls(abFlag: Boolean): Void
	{
		_disableControls = abFlag;
		HoursSlider.thumb.disabled = HoursSlider.track.disabled = ButtonRect.AcceptMouseButton.disabled = abFlag;
	}

	public function get disableControls(): Boolean
	{
		return _disableControls;
	}

	public function setSliderValue(value: Number): Void
	{
		HoursSlider.value = value;
		sliderChange();
	}

	public function modifySliderValue(aiDelta: Number): Void
	{
		HoursSlider.value += aiDelta;
		sliderChange();
	}

	public function getSliderValue(): Number
	{
		return Math.floor(HoursSlider.value);
	}


	private function handleInput(details: gfx.ui.InputDetails, pathToFocus: Array): Boolean
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

		if (!bWaiting && bInArea)
		{
			var barCenterX: Number = progressBar._x;
			var barCenterY: Number = progressBar._y;
			var angle: Number = Math.atan2(_ymouse - barCenterY, _xmouse - barCenterX) * 180 / Math.PI;
			var clickHour: Number = Math.round(((angle + 90) % 360) * 24 / 360);
			if (clickHour <= 0)
				clickHour += 24;
			
			changeWaitTime(clickHour);
		}
	}

	private function onMouseWheel(aiWheelVal: Number): Void
	{
		if (!disableControls)
			modifySliderValue(aiWheelVal);
	}

	// DLL Call
	public function onStickLeft(afX: Number, afY: Number): Void
	{
		var magnitude: Number = Math.sqrt(afX * afX + afY * afY);
		if (magnitude < 0.9)
			return;

		if (!bWaiting && !disableControls)
		{
			var angleRad: Number = Math.atan2(-afY, afX);
			var angleDeg: Number = angleRad * 180 / Math.PI;

			var clickHour: Number = Math.round(((angleDeg + 450) % 360) * 24 / 360);
			if (clickHour <= 0)
				clickHour = 24;

			changeWaitTime(clickHour);
		}
	}

	private function sliderChange(event: Object): Void
	{
		hours = getSliderValue();
		WaitTime.SetText(hours);

		GameDelegate.call("PlaySound", ["UIMenuPrevNext"]);

		if (!bWaiting)
		{
			drawBar(iHour, hours);
			setWaitTime();

			var iTimeUse: Number = iHour + hours;
			if (iTimeUse > 24)
				iTimeUse -= 24;

			progressBar.gotoAndStop(iTimeUse);
		}
	}

	private function onOKPress(event: Object): Void
	{
		if (!disableControls)
		{
			showCursor(true);

			bWaiting = disableControls = true;
			GameDelegate.call("OK", [getSliderValue()]);
		}
	}

	private function onCancelPress(event: Object): Void
	{
		bWaiting = false;
		GameDelegate.call("Cancel", []);
	}

	private function handleRollActions(): Void
	{
		if (!bWaiting)
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
	public function setTimeAndDate(): Void
	{
		var sameHour: Boolean = iHour == arguments[1];
		iHour = arguments[1];

		if (bWaiting)
			drawBar(iHour, hours);
		else
		{
			minute = arguments[2];

			sliderChange();

			if (!sameHour || !bDoOnce)
			{
				bDoOnce = true;
				progressBar.gotoAndStop(iHour + 1);
			}
		}

		setTimeText(arguments);
	}

	// DLL Call
	public function updateWeather(): Void
	{
		iconWeather._alpha = 100;
		iconWeather.gotoAndStop(Number(arguments) + 1);
	}

	// API
	public function SetSleeping(aSleeping: Boolean): Void
	{
		QuestionInstance.SetText(aSleeping ? "$fta_REST" : "$fta_WAIT");
	}

	// API
	public function SetCurrentTime(aTime: String): Void
	{

	}

	private function changeWaitTime(clickHour: Number): Void
	{
		if (iOldClickHour != clickHour)
		{
			iOldClickHour = clickHour;

			// The following needs to be here two times
			var hoursDifference: Number = (clickHour - iHour) - 12;
			if (hoursDifference <= 0)
				hoursDifference += 24;

			setSliderValue(hoursDifference);
		}
	}

	private function setTimeText(args: Array): Void
	{
		var timeString: String = "";
		if (is24Clock)
		{
			var leadingZero: String = (useLeadingZero && iHour < 10) ? "0" : "";
			timeString = leadingZero + iHour + ":" + minute;
		}
		else
		{
			var time: Number = args[0];
			var suffix: String = (iHour < 12) ? suffixAM : suffixPM;
			var leadingZero: String = (useLeadingZero && (time > 0 && time < 10)) ? "0" : "";

			timeString = leadingZero + args[0] + ":" + minute + " " + suffix;
		}

		CurTime.SetText(timeString);
		CurrentDate.SetText(args[4] + ", " + args[3] + ". " + args[5] + " " + args[6]);
	}

	private function setWaitTime(): Void
	{
		var newHour: Number = iHour + hours;
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
		Mouse[bShow ? "show" : "hide"]();
		// Following triggers an event inside the DLL
		skse.SendModEvent("MWMShowMouseCursor", "", bShow ? 1 : 0);
		cursor._alpha = bShow ? 0 : 100;
	}

	private function drawBar(currentTime: Number, waitTime: Number): Void
	{
		var centerX: Number = 239.85;
		var centerY: Number = 239.85;
		var radius: Number = 250;
		var fraction: Number = 15;

		var startAngle: Number = -270 + (currentTime * fraction);
		var endAngle: Number = startAngle + (waitTime * fraction);
		var angleStep: Number = 5;

		container.clear();
		container.beginFill(0x828282, 100);
		container.moveTo(centerX, centerY);

		for (var angle: Number = startAngle; angle <= endAngle; angle += angleStep)
		{
			var radians: Number = (angle * Math.PI) / 180;
			container.lineTo(centerX + (radius * Math.cos(radians)), centerY + (radius * Math.sin(radians)));
		}

		container.lineTo(centerX, centerY);
		container.endFill();
	}

	private function convertTo12HourFormat(hour: Number): String
	{
		var hour12: Number = hour % 12;
		return (useLeadingZero && (hour12 > 0 && hour12 < 10) ? "0" : "") + (hour12 == 0 ? "12" : hour12.toString());
	}
}