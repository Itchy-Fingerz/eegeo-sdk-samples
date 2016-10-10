//Copyright eeGeo Ltd (2012-2014), All Rights Reserved

package com.eegeo.mobilesdkharness;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.WindowManager;
import com.vuforia.Vuforia;


public class BackgroundThreadActivity extends MainActivity
{
	private EegeoSurfaceView m_surfaceView;
	private SurfaceHolder m_surfaceHolder;
	private long m_nativeAppWindowPtr;
	private ThreadedUpdateRunner m_threadedRunner;
	private Thread m_updater;
	private boolean m_isInVRMode;
	private VRModule m_vrModule;
	
	// Constants for Hiding/Showing Loading dialog
    static final int HIDE_LOADING_DIALOG = 0;
    static final int SHOW_LOADING_DIALOG = 1;
    
    private Object m_shutdownLock = new Object();
    
    // Vuforia initialization flags:
    private int m_vuforiaFlags = 0;
    
    private static View m_loadingDialogContainer;
    // Application status constants:
    private static final int APPSTATUS_UNINITED = -1;
    private static final int APPSTATUS_INIT_APP = 0;
    private static final int APPSTATUS_INIT_VUFORIA = 1;
    private static final int APPSTATUS_INIT_TRACKER = 2;
    private static final int APPSTATUS_INIT_APP_AR = 3;
    private static final int APPSTATUS_LOAD_TRACKER = 4;
    private static final int APPSTATUS_INITED = 5;
    private static final int APPSTATUS_CAMERA_RUNNING = 7;
    
    final static int CAMERA_DIRECTION_DEFAULT = 0;
    // Keeps track of the current camera
    int m_currentCamera = CAMERA_DIRECTION_DEFAULT;
    // Constant representing invalid screen orientation to trigger a query:
    private static final int INVALID_SCREEN_ROTATION = -1;
    
    // Last detected screen rotation:
    private int m_lastScreenRotation = INVALID_SCREEN_ROTATION;
    // The current application status:
    private int m_appStatus = APPSTATUS_UNINITED;
    
    // Display size of the device:
    private int m_screenWidth = 0;
    private int m_screenHeight = 0;
    
    // The async tasks to initialize the Vuforia SDK:
    private InitVuforiaTask m_initVuforiaTask;
    private LoadTrackerTask m_loadTrackerTask;

	static {
		System.loadLibrary("eegeo-sdk-samples");
		System.loadLibrary("Vuforia");
	}
	
	/**
     * Creates a handler to update the status of the Loading Dialog from an UI
     * Thread
     */
    static class LoadingDialogHandler extends Handler
    {
        public void handleMessage(Message msg)
        { 
            if (msg.what == SHOW_LOADING_DIALOG)
            {
                BackgroundThreadActivity.m_loadingDialogContainer
                    .setVisibility(View.VISIBLE);
                
            } else if (msg.what == HIDE_LOADING_DIALOG)
            {
            	BackgroundThreadActivity.m_loadingDialogContainer.setVisibility(View.GONE);
            }
        }
    }
	private Handler loadingDialogHandler = new LoadingDialogHandler();
    
    /** An async task to initialize Vuforia asynchronously. */
    private class InitVuforiaTask extends AsyncTask<Void, Integer, Boolean>
    {
        // Initialize with invalid value:
        private int mProgressValue = -1;
        
        
        protected Boolean doInBackground(Void... params)
        {
            // Prevent the onDestroy() method to overlap with initialization:
            synchronized (m_shutdownLock)
            {
                Vuforia.setInitParameters(BackgroundThreadActivity.this, m_vuforiaFlags, "AQJPDKr/////AAAAGddZNNN7v0bggrFUaxqVUVN0n+1qqW3B9qOH7woVAjB2h/lc23NU8Oh+1dt4qyuelrh3M+55/jWcazJEoYEXbNqSlZeaxV6jGgggEb5qr6kIUK8ua8Ve8yGpi8rO5M7F0EwH4c1n8kL1avrBfWwo50zEs305oHpRU50HdyoGf9lD/jwOyw8uVOWcPc0tXalTgOsLoHN63HgOT1q+Rp+5zfec0RzCEZTCAGLUqYNmg+N3wUeULwC5EbKBKAn8NMJJZRspIrePuJE6fBWYWJKXTQLDCj/fRrCPg6afYipLi3Nr3bbyrrriMCkBuVvjS7NmBujEpWJuYL2roTaZ62VpIEE7wpmaACUTOLrwnFM58Hov");
                
                do
                {
                    // Vuforia.init() blocks until an initialization step is
                    // complete, then it proceeds to the next step and reports
                    // progress in percents (0 ... 100%).
                    // If Vuforia.init() returns -1, it indicates an error.
                    // Initialization is done when progress has reached 100%.
                    mProgressValue = Vuforia.init();
                    
                    // Publish the progress value:
                    publishProgress(mProgressValue);
                    
                    // We check whether the task has been canceled in the
                    // meantime (by calling AsyncTask.cancel(true)).
                    // and bail out if it has, thus stopping this thread.
                    // This is necessary as the AsyncTask will run to completion
                    // regardless of the status of the component that
                    // started is.
                } while (!isCancelled() && mProgressValue >= 0
                    && mProgressValue < 100);
                
                return (mProgressValue > 0);
            }
        }
        
        
        protected void onProgressUpdate(Integer... values)
        {
            // Do something with the progress value "values[0]", e.g. update
            // splash screen, progress bar, etc.
        }
        
        
        protected void onPostExecute(Boolean result)
        {
            // Done initializing Vuforia, proceed to next application
            // initialization status:
            if (result)
            {
            	Log.e("Eegeo-AR","InitVuforiaTask::onPostExecute: Vuforia "
                    + "initialization successful");
                
                //updateApplicationStatus(APPSTATUS_INIT_TRACKER);
            	initTracker();
            } else
            {
                // Create dialog box for display error:
                AlertDialog dialogError = new AlertDialog.Builder(
                    BackgroundThreadActivity.this).create();
                
                dialogError.setButton(DialogInterface.BUTTON_POSITIVE, "OK",
                    new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int which)
                        {
                            // Exiting application:
                            System.exit(1);
                        }
                    });
                // Show dialog box with error message:
                dialogError.setMessage("Log error");
                dialogError.show();
            }
        }
    }
    
    /** An async task to load the tracker data asynchronously. */
    private class LoadTrackerTask extends AsyncTask<Void, Integer, Boolean>
    {
        protected Boolean doInBackground(Void... params)
        {
            // Prevent the onDestroy() method to overlap:
            synchronized (m_shutdownLock)
            {
                // Load the tracker data set:
                return (NativeJniCalls.loadTrackerData() > 0);
            }
        }
        
        
        protected void onPostExecute(Boolean result)
        {
        	Log.e("Eegeo-AR","LoadTrackerTask::onPostExecute: execution "
                + (result ? "successful" : "failed"));
            
            if (result)
            {
                // Done loading the tracker, update application status:
                updateApplicationStatus(APPSTATUS_INITED);
                loadingDialogHandler.sendEmptyMessage(HIDE_LOADING_DIALOG);
            } else
            {
                // Create dialog box for display error:
                AlertDialog dialogError = new AlertDialog.Builder(
                    BackgroundThreadActivity.this).create();
                
                dialogError.setButton(DialogInterface.BUTTON_POSITIVE, "Close",
                    new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int which)
                        {
                            // Exiting application:
                            System.exit(1);
                        }
                    });
                
                // Show dialog box with error message:
                dialogError.setMessage("Failed to load tracker data.");
                dialogError.show();
            }
        }
    }
	
	/** Stores screen dimensions */
    private void storeScreenDimensions()
    {
        // Query display dimensions:
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        m_screenWidth = metrics.widthPixels;
        m_screenHeight = metrics.heightPixels;
    }
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main);

		m_surfaceView = (EegeoSurfaceView)findViewById(R.id.surface);
		m_surfaceView.getHolder().addCallback(this);
		m_surfaceView.setActivity(this);

		m_vrModule = new VRModule(this);
		DisplayMetrics dm = getResources().getDisplayMetrics();
		final float dpi = dm.ydpi;
		final Activity activity = this;
		
		 // Gets a reference to the loading dialog
        m_loadingDialogContainer = findViewById(R.id.loading_indicator);
		 //mTextures = new Vector<Texture>();
	     //loadTextures();
	        
	     // Configure Vuforia to use OpenGL ES 2.0
	     m_vuforiaFlags = Vuforia.GL_20;
		// Update the application status to start initializing application:
        //updateApplicationStatus(APPSTATUS_INIT_APP);
	     initApplication();

		m_threadedRunner = new ThreadedUpdateRunner(false);
		m_updater = new Thread(m_threadedRunner);
		m_updater.start();

		m_threadedRunner.blockUntilThreadStartedRunning();

		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				m_nativeAppWindowPtr = NativeJniCalls.createNativeCode(activity, getAssets(), dpi);

				if(m_nativeAppWindowPtr == 0)
				{
					throw new RuntimeException("Failed to start native code.");
				}
			}
		});
	}
	

	@SuppressLint("InlinedApi")
	private void setScreenSettings()
	{
		
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		if(android.os.Build.VERSION.SDK_INT<16)
			getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
		else if(android.os.Build.VERSION.SDK_INT<19)
			getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_FULLSCREEN);
		else
			getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_FULLSCREEN);
		
	}
	
	public void enterVRMode()
	{
		if(!m_isInVRMode)
		{
			this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
			m_isInVRMode = true;
		}
	}
	
	public void exitVRMode()
	{
		if(m_isInVRMode)
		{
			this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED);
			m_isInVRMode = false;
		}
	}

	public void runOnNativeThread(Runnable runnable)
	{
		m_threadedRunner.postTo(runnable);
	}
    
    /** Native method for starting / stopping off target tracking */
    private native boolean startExtendedTracking();
    
    
    private native boolean stopExtendedTracking();

	@Override
	protected void onResume()
	{
		super.onResume();
		// Vuforia-specific resume operation
        Vuforia.onResume();
		
		setScreenSettings();
		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				NativeJniCalls.resumeNativeCode();
				m_threadedRunner.start();
				
				if(m_surfaceHolder != null && m_surfaceHolder.getSurface() != null)
				{
					NativeJniCalls.setNativeSurface(m_surfaceHolder.getSurface());
					NativeJniCalls.updateCardboardProfile(m_vrModule.getUpdatedCardboardProfile());
				}
			}
		});
	}

	@Override
	protected void onPause()
	{
		super.onPause();
		
		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				m_threadedRunner.stop();
				NativeJniCalls.pauseNativeCode();
			}
		});
        
        // Vuforia-specific pause operation
        Vuforia.onPause();
	}
	
	 /** Native function to deinitialize the application. */
   // private native void deinitApplicationNative();

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		
		m_vrModule.stopTracker();
		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				m_threadedRunner.stop();
				NativeJniCalls.destroyNativeCode();
				m_threadedRunner.destroyed();
			}
		});

		m_threadedRunner.blockUntilThreadHasDestroyedPlatform();
		m_nativeAppWindowPtr = 0;
		// Cancel potentially running tasks
        if (m_initVuforiaTask != null
            && m_initVuforiaTask.getStatus() != InitVuforiaTask.Status.FINISHED)
        {
            m_initVuforiaTask.cancel(true);
            m_initVuforiaTask = null;
        }
        
        if (m_loadTrackerTask != null
            && m_loadTrackerTask.getStatus() != LoadTrackerTask.Status.FINISHED)
        {
            m_loadTrackerTask.cancel(true);
            m_loadTrackerTask = null;
        }
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder)
	{
		  // Call native function to initialize rendering:
        //NativeJniCalls.initVuforiaRendering();
        
        // Call Vuforia function to (re)initialize rendering after first use
        // or after OpenGL ES context was lost (e.g. after onPause/onResume):
        Vuforia.onSurfaceCreated();
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder)
	{

		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				m_threadedRunner.stop();
			}
		});
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, final int width, final int height)
	{
        
        // Call Vuforia function to handle render surface size changes:
        Vuforia.onSurfaceChanged(width, height);
		final SurfaceHolder h = holder;
		
		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				m_surfaceHolder = h;
				if(m_surfaceHolder != null) 
				{
					// Call native function to update rendering when render surface
			        // parameters have changed:
			        NativeJniCalls.updateVuforiaRendering(width, height);
					NativeJniCalls.setNativeSurface(m_surfaceHolder.getSurface());
					m_threadedRunner.start();
					NativeJniCalls.updateCardboardProfile(m_vrModule.getUpdatedCardboardProfile());
				}
			}
		});
	}

	private class ThreadedUpdateRunner implements Runnable
	{
		private long m_endOfLastFrameNano;
		private boolean m_running;
		private Handler m_nativeThreadHandler;
		private float m_frameThrottleDelaySeconds;
		private boolean m_destroyed;

		public ThreadedUpdateRunner(boolean running)
		{
			m_endOfLastFrameNano = System.nanoTime();
			m_running = false;
			m_destroyed = false;

			float targetFramesPerSecond = 30.f;
			m_frameThrottleDelaySeconds = 1.f/targetFramesPerSecond;
		}

		synchronized void blockUntilThreadStartedRunning()
		{
			while(m_nativeThreadHandler == null);
		}

		synchronized void blockUntilThreadHasDestroyedPlatform()
		{
			while(!m_destroyed);
		}

		public void postTo(Runnable runnable)
		{
			m_nativeThreadHandler.post(runnable);
		}

		public void start()
		{
			m_running = true;
		}

		public void stop()
		{
			m_running = false;
		}

		public void destroyed()
		{
			m_destroyed = true;
		}

		public void run()
		{
			Looper.prepare();
			m_nativeThreadHandler = new Handler();

			while(true)
			{
				runOnNativeThread(new Runnable()
				{
					public void run()
					{
						long timeNowNano = System.nanoTime();
						long nanoDelta = timeNowNano - m_endOfLastFrameNano;
						float deltaSeconds = (float)((double)nanoDelta / 1e9);
						
						if(deltaSeconds > m_frameThrottleDelaySeconds)
						{
							if(m_running)
							{
								m_vrModule.updateNativeCode(deltaSeconds);
								//updateRenderView();
							}
							else
							{
								SystemClock.sleep(200);
							}

							m_endOfLastFrameNano = timeNowNano;
						}

						runOnNativeThread(this);
					}
				});

				Looper.loop();
			}
		}
	}
	
	/**
     * NOTE: this method is synchronized because of a potential concurrent
     * access by onResume() and InitVuforiaTask.onPostExecute().
     */
    private synchronized void updateApplicationStatus(int appStatus)
    {
        // Exit if there is no change in status:
        if (m_appStatus == appStatus)
            return;
        
        // Store new status value:
        m_appStatus = appStatus;
        
        // Execute application state-specific actions:
        switch (m_appStatus)
        {
            case APPSTATUS_INITED:
                // Hint to the virtual machine that it would be a good time to
                // run the garbage collector:
                //
                // NOTE: This is only a hint. There is no guarantee that the
                // garbage collector will actually be run.
                System.gc();
                
                // Native post initialization:
                NativeJniCalls.onVuforiaInitializedNative();
                // Start the camera:
                // camera already started from native code from above method
                m_appStatus = APPSTATUS_CAMERA_RUNNING;
                break;
            
            default:
                throw new RuntimeException("Invalid application state");
        }
    }
    
    
    /** Initialize application GUI elements that are not related to AR. */
    private void initApplication()
    {
        //setActivityPortraitMode(true);
        
        // Query display dimensions:
        storeScreenDimensions();
        
        // As long as this window is visible to the user, keep the device's
        // screen turned on and bright:
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
    
    
    /**
     * Updates projection matrix and viewport after a screen rotation change was
     * detected.
     */
    public void updateRenderView()
    {
        int currentScreenRotation = getWindowManager().getDefaultDisplay()
            .getRotation();
        if (currentScreenRotation != m_lastScreenRotation)
        {
            // Set projection matrix if there is already a valid one:
            if (Vuforia.isInitialized()
                && (m_appStatus == APPSTATUS_CAMERA_RUNNING))
            {
                Log.d("EEgeo-AR","updateRenderView");
                
                // Query display dimensions:
                storeScreenDimensions();
                
                // Update viewport via renderer:
                NativeJniCalls.updateVuforiaRendering(m_screenWidth, m_screenHeight);
                
                // Update projection matrix:
                //setProjectionMatrix(); Already done from he above native method
                
                // Cache last rotation used for setting projection matrix:
                m_lastScreenRotation = currentScreenRotation;
            }
        }
    }

	@Override
	public void initVuforia()
	{
		loadingDialogHandler.sendEmptyMessage(SHOW_LOADING_DIALOG);
		 try
         {
             m_initVuforiaTask = new InitVuforiaTask();
             m_initVuforiaTask.execute();
         } catch (Exception e)
         {
             Log.e("Eegeo-AR","Initializing Vuforia SDK failed");
         }
	}
	
	private void initTracker()
	{
		 if (NativeJniCalls.initTracker() > 0)
         {
			loadTracker();
         }
	}
	
	private void loadTracker()
	{
		try
        {
            m_loadTrackerTask = new LoadTrackerTask();
            m_loadTrackerTask.execute();
        } catch (Exception e)
        {
        	 Log.e("Eegeo-AR","Loading tracking data set failed");
        }
	}

	@Override
	public void deInitVuforia()
	{
        // Ensure that all asynchronous operations to initialize Vuforia
        // and loading the tracker datasets do not overlap:
        synchronized (m_shutdownLock)
        {
            
           /* // Do application deinitialization in native code:
            deinitApplicationNative();
            
            // Destroy the tracking data set:
            destroyTrackerData();
            
            // Deinit the tracker:
            deinitTracker();*/
            
            // Deinitialize Vuforia SDK:
            Vuforia.deinit();
        }
        
        System.gc();
	}
    
}