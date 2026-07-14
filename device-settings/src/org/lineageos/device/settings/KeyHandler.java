/*
 * Copyright (C) 2018-2023 crDroid Android Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.lineageos.device.settings;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.hardware.input.InputManager;
import android.os.UserHandle;
import android.util.Log;
import android.view.KeyEvent;

import androidx.annotation.Keep;

import com.android.internal.os.DeviceKeyHandler;

import java.util.Arrays;

import org.lineageos.device.settings.Constants;
import org.lineageos.device.settings.SliderControllerBase;
import org.lineageos.device.settings.slider.NotificationController;
import org.lineageos.device.settings.slider.FlashlightController;
import org.lineageos.device.settings.slider.BrightnessController;
import org.lineageos.device.settings.slider.RotationController;
import org.lineageos.device.settings.slider.RingerController;
import org.lineageos.device.settings.slider.NotificationRingerController;

@Keep
public class KeyHandler implements DeviceKeyHandler {
    private static final String TAG = KeyHandler.class.getSimpleName();
    private static final String PACKAGE_NAME = "org.lineageos.device.settings";

    private final Context mContext;
    private final NotificationController mNotificationController;
    private final FlashlightController mFlashlightController;
    private final BrightnessController mBrightnessController;
    private final RotationController mRotationController;
    private final RingerController mRingerController;
    private final NotificationRingerController mNotificationRingerController;

    private SliderControllerBase mSliderController;

    private final InputManager mInputManager;

    private final BroadcastReceiver mSliderUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int usage = intent.getIntExtra(Constants.EXTRA_SLIDER_USAGE, 0);
            int[] actions = intent.getIntArrayExtra(Constants.EXTRA_SLIDER_ACTIONS);

            Log.i(TAG, "update usage " + usage + " with actions " +
                    Arrays.toString(actions));

            if (mSliderController != null) {
                mSliderController.reset();
            }

            switch (usage) {
                case NotificationController.ID:
                    mSliderController = mNotificationController;
                    mSliderController.update(actions);
                    break;
                case FlashlightController.ID:
                    mSliderController = mFlashlightController;
                    mSliderController.update(actions);
                    break;
                case BrightnessController.ID:
                    mSliderController = mBrightnessController;
                    mSliderController.update(actions);
                    break;
                case RotationController.ID:
                    mSliderController = mRotationController;
                    mSliderController.update(actions);
                    break;
                case RingerController.ID:
                    mSliderController = mRingerController;
                    mSliderController.update(actions);
                    break;
                case NotificationRingerController.ID:
                    mSliderController = mNotificationRingerController;
                    mSliderController.update(actions);
                    break;
            }

            mSliderController.restoreState(context, false);
        }
    };

    public KeyHandler(Context context) {
        mContext = context;

        mNotificationController = new NotificationController(mContext);
        mFlashlightController = new FlashlightController(mContext);
        mBrightnessController = new BrightnessController(mContext);
        mRotationController = new RotationController(mContext);
        mRingerController = new RingerController(mContext);
        mNotificationRingerController = new NotificationRingerController(mContext);

        mContext.registerReceiver(mSliderUpdateReceiver,
                new IntentFilter(Constants.ACTION_UPDATE_SLIDER_SETTINGS));

        mInputManager = mContext.getSystemService(InputManager.class);

        initSliderFromPrefs();
    }

    private void initSliderFromPrefs() {
        try {
            // KeyHandler runs in system_server, so use createPackageContext
            // to read DeviceSettings SharedPreferences correctly
            Context appContext = mContext.createPackageContext(PACKAGE_NAME,
                    Context.CONTEXT_IGNORE_SECURITY);
            Resources res = appContext.getResources();
            SharedPreferences prefs = appContext.getSharedPreferences(
                    PACKAGE_NAME + "_preferences", Context.MODE_PRIVATE);

            String usage = prefs.getString(Constants.KEY_NOTIF_SLIDER_USAGE,
                    res.getString(R.string.config_defaultNotificationSliderUsage));
            if (usage == null) {
                usage = Constants.NOTIF_SLIDER_FOR_NOTIFICATION;
            }

            int defaultsResId = getDefaultResIdForUsage(usage);
            if (defaultsResId == 0) {
                return;
            }

            String[] defaults = res.getStringArray(defaultsResId);
            if (defaults.length != 3) {
                return;
            }

            String actionTop = prefs.getString(
                    Constants.KEY_NOTIF_SLIDER_ACTION_TOP, defaults[0]);
            String actionMiddle = prefs.getString(
                    Constants.KEY_NOTIF_SLIDER_ACTION_MIDDLE, defaults[1]);
            String actionBottom = prefs.getString(
                    Constants.KEY_NOTIF_SLIDER_ACTION_BOTTOM, defaults[2]);

            int usageInt = Integer.parseInt(usage);
            int[] actions = new int[] {
                    Integer.parseInt(actionTop),
                    Integer.parseInt(actionMiddle),
                    Integer.parseInt(actionBottom)
            };

            Log.i(TAG, "initSliderFromPrefs: usage=" + usage + " actions=" +
                    Arrays.toString(actions));

            switch (usageInt) {
                case NotificationController.ID:
                    mSliderController = mNotificationController;
                    break;
                case FlashlightController.ID:
                    mSliderController = mFlashlightController;
                    break;
                case BrightnessController.ID:
                    mSliderController = mBrightnessController;
                    break;
                case RotationController.ID:
                    mSliderController = mRotationController;
                    break;
                case RingerController.ID:
                    mSliderController = mRingerController;
                    break;
                case NotificationRingerController.ID:
                    mSliderController = mNotificationRingerController;
                    break;
            }

            if (mSliderController != null) {
                mSliderController.update(actions);
                mSliderController.restoreState(mContext, false);
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed to init slider from prefs", e);
        }
    }

    private static int getDefaultResIdForUsage(String usage) {
        switch (usage) {
            case Constants.NOTIF_SLIDER_FOR_NOTIFICATION:
                return R.array.config_defaultSliderActionsForNotification;
            case Constants.NOTIF_SLIDER_FOR_FLASHLIGHT:
                return R.array.config_defaultSliderActionsForFlashlight;
            case Constants.NOTIF_SLIDER_FOR_BRIGHTNESS:
                return R.array.config_defaultSliderActionsForBrightness;
            case Constants.NOTIF_SLIDER_FOR_ROTATION:
                return R.array.config_defaultSliderActionsForRotation;
            case Constants.NOTIF_SLIDER_FOR_RINGER:
                return R.array.config_defaultSliderActionsForRinger;
            case Constants.NOTIF_SLIDER_FOR_NOTIFICATION_RINGER:
                return R.array.config_defaultSliderActionsForNotificationRinger;
            default:
                return 0;
        }
    }

    public KeyEvent handleKeyEvent(KeyEvent event) {
        if (event.getAction() != KeyEvent.ACTION_DOWN) {
            return event;
        }

        if (!mInputManager.getInputDevice(event.getDeviceId()).getName().equals("oplus,hall_tri_state_key")) {
            return event;
        }

        if (mSliderController == null) {
            Log.w(TAG, "Slider controller not initialized yet");
            return event;
        }

        mSliderController.processEvent(mContext);

        return null;
    }
}
