//Copyright eeGeo Ltd (2012-2014), All Rights Reserved

package com.eegeo.mobilesdkharness;

import java.util.Vector;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
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
	
    // Focus mode constants:
    private static final int FOCUS_MODE_NORMAL = 0;
    private static final int FOCUS_MODE_CONTINUOUS_AUTO = 1;
	// Constants for Hiding/Showing Loading dialog
    static final int HIDE_LOADING_DIALOG = 0;
    static final int SHOW_LOADING_DIALOG = 1;
    
    private Object mShutdownLock = new Object();
    
    // Vuforia initialization flags:
    private int mVuforiaFlags = 0;
    private boolean mContAutofocus = false;
    // The textures we will use for rendering:
    private Vector<Texture> mTextures;
    
    private static View mLoadingDialogContainer;
    // Application status constants:
    private static final int APPSTATUS_UNINITED = -1;
    private static final int APPSTATUS_INIT_APP = 0;
    private static final int APPSTATUS_INIT_VUFORIA = 1;
    private static final int APPSTATUS_INIT_TRACKER = 2;
    private static final int APPSTATUS_INIT_APP_AR = 3;
    private static final int APPSTATUS_LOAD_TRACKER = 4;
    private static final int APPSTATUS_INITED = 5;
    private static final int APPSTATUS_CAMERA_STOPPED = 6;
    private static final int APPSTATUS_CAMERA_RUNNING = 7;
    
    final static int CAMERA_DIRECTION_DEFAULT = 0;
    final static int CAMERA_DIRECTION_BACK = 1;
    final static int CAMERA_DIRECTION_FRONT = 2;
    // Keeps track of the current camera
    int mCurrentCamera = CAMERA_DIRECTION_DEFAULT;
    // Constant representing invalid screen orientation to trigger a query:
    private static final int INVALID_SCREEN_ROTATION = -1;
    
    // Last detected screen rotation:
    private int mLastScreenRotation = INVALID_SCREEN_ROTATION;
    // The current application status:
    private int mAppStatus = APPSTATUS_UNINITED;
    
    // Display size of the device:
    private int mScreenWidth = 0;
    private int mScreenHeight = 0;
    
    // The async tasks to initialize the Vuforia SDK:
    private InitVuforiaTask mInitVuforiaTask;
    private LoadTrackerTask mLoadTrackerTask;
	
    /** Native function for initializing the renderer. */
    public native void initRendering();
    
    
    /** Native function to update the renderer. */
    public native void updateRendering(int width, int height);

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
                BackgroundThreadActivity.mLoadingDialogContainer
                    .setVisibility(View.VISIBLE);
                
            } else if (msg.what == HIDE_LOADING_DIALOG)
            {
            	BackgroundThreadActivity.mLoadingDialogContainer.setVisibility(View.GONE);
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
            synchronized (mShutdownLock)
            {
                Vuforia.setInitParameters(BackgroundThreadActivity.this, mVuforiaFlags, "AQJPDKr/////AAAAGddZNNN7v0bggrFUaxqVUVN0n+1qqW3B9qOH7woVAjB2h/lc23NU8Oh+1dt4qyuelrh3M+55/jWcazJEoYEXbNqSlZeaxV6jGgggEb5qr6kIUK8ua8Ve8yGpi8rO5M7F0EwH4c1n8kL1avrBfWwo50zEs305oHpRU50HdyoGf9lD/jwOyw8uVOWcPc0tXalTgOsLoHN63HgOT1q+Rp+5zfec0RzCEZTCAGLUqYNmg+N3wUeULwC5EbKBKAn8NMJJZRspIrePuJE6fBWYWJKXTQLDCj/fRrCPg6afYipLi3Nr3bbyrrriMCkBuVvjS7NmBujEpWJuYL2roTaZ62VpIEE7wpmaACUTOLrwnFM58Hov");
                
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
                
                updateApplicationStatus(APPSTATUS_INIT_TRACKER);
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
            synchronized (mShutdownLock)
            {
                // Load the tracker data set:
                return (loadTrackerData() > 0);
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
        mScreenWidth = metrics.widthPixels;
        mScreenHeight = metrics.heightPixels;
    }
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		setContentView(R.layout.activity_main);

		m_surfaceView = (EegeoSurfaceView)findViewById(R.id.surface);
		m_surfaceView.getHolder().addCallback(this);
		m_surfaceView.setActivity(this);

		DisplayMetrics dm = getResources().getDisplayMetrics();
		final float dpi = dm.ydpi;
		final Activity activity = this;

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
		
        // Gets a reference to the loading dialog
        mLoadingDialogContainer = findViewById(R.id.loading_indicator);
		 mTextures = new Vector<Texture>();
	     loadTextures();
	        
	     // Configure Vuforia to use OpenGL ES 2.0
	     mVuforiaFlags = Vuforia.GL_20;
		// Update the application status to start initializing application:
        updateApplicationStatus(APPSTATUS_INIT_APP);
	}
	
	/**
     * We want to load specific textures from the APK, which we will later use
     * for rendering.
     */
    private void loadTextures()
    {
        mTextures.add(Texture.loadTextureFromApk("TextureTeapotBrass.png",
            getAssets()));
        mTextures.add(Texture.loadTextureFromApk("TextureTeapotBlue.png",
            getAssets()));
        mTextures.add(Texture.loadTextureFromApk("TextureTeapotRed.png",
            getAssets()));
        mTextures
            .add(Texture.loadTextureFromApk("Buildings.jpeg", getAssets()));
    }

	public void runOnNativeThread(Runnable runnable)
	{
		m_threadedRunner.postTo(runnable);
	}
	
	 /** Native tracker initialization and deinitialization. */
    public native int initTracker();
    
    
    public native void deinitTracker();
    
    
    /** Native functions to load and destroy tracking data. */
    public native int loadTrackerData();
    
    
    public native void destroyTrackerData();
    
    
    /** Native sample initialization. */
    public native void onVuforiaInitializedNative();
    
    
    /** Native methods for starting and stopping the desired camera. */
    private native void startCamera(int camera);
    
    
    private native void stopCamera();
    
    
    /**
     * Native method for setting / updating the projection matrix for AR content
     * rendering
     */
    private native void setProjectionMatrix();
    
    
    /** Native method for starting / stopping off target tracking */
    private native boolean startExtendedTracking();
    
    
    private native boolean stopExtendedTracking();

	@Override
	protected void onResume()
	{
		super.onResume();
		// Vuforia-specific resume operation
        Vuforia.onResume();
		
		runOnNativeThread(new Runnable()
		{
			public void run()
			{
				NativeJniCalls.resumeNativeCode();
				m_threadedRunner.start();
				
				if(m_surfaceHolder != null && m_surfaceHolder.getSurface() != null)
				{
					NativeJniCalls.setNativeSurface(m_surfaceHolder.getSurface());
				}
			}
		});
		
		// We may start the camera only if the Vuforia SDK has already been
        // initialized
        if (mAppStatus == APPSTATUS_CAMERA_STOPPED)
        {
            updateApplicationStatus(APPSTATUS_CAMERA_RUNNING);
        }
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
        
        if (mAppStatus == APPSTATUS_CAMERA_RUNNING)
        {
            updateApplicationStatus(APPSTATUS_CAMERA_STOPPED);
        }
        
        // Vuforia-specific pause operation
        Vuforia.onPause();
	}
	
	 /** Native function to deinitialize the application. */
    private native void deinitApplicationNative();

	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		
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
        if (mInitVuforiaTask != null
            && mInitVuforiaTask.getStatus() != InitVuforiaTask.Status.FINISHED)
        {
            mInitVuforiaTask.cancel(true);
            mInitVuforiaTask = null;
        }
        
        if (mLoadTrackerTask != null
            && mLoadTrackerTask.getStatus() != LoadTrackerTask.Status.FINISHED)
        {
            mLoadTrackerTask.cancel(true);
            mLoadTrackerTask = null;
        }
        
        // Ensure that all asynchronous operations to initialize Vuforia
        // and loading the tracker datasets do not overlap:
        synchronized (mShutdownLock)
        {
            
            // Do application deinitialization in native code:
            deinitApplicationNative();
            
            // Destroy the tracking data set:
            destroyTrackerData();
            
            // Deinit the tracker:
            deinitTracker();
            
            // Deinitialize Vuforia SDK:
            Vuforia.deinit();
        }
        
        System.gc();
	}
	
	@Override
	public void surfaceCreated(SurfaceHolder holder)
	{
		//nothing to do
		  // Call native function to initialize rendering:
        initRendering();
        
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
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height)
	{
		// Call native function to update rendering when render surface
        // parameters have changed:
        updateRendering(width, height);
        
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
					NativeJniCalls.setNativeSurface(m_surfaceHolder.getSurface());
					m_threadedRunner.start();
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
								NativeJniCalls.updateNativeCode(deltaSeconds);
								updateRenderView();
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
        if (mAppStatus == appStatus)
            return;
        
        // Store new status value:
        mAppStatus = appStatus;
        
        // Execute application state-specific actions:
        switch (mAppStatus)
        {
            case APPSTATUS_INIT_APP:
                // Initialize application elements that do not rely on Vuforia
                // initialization:
                initApplication();
                
                // Proceed to next application initialization status:
                updateApplicationStatus(APPSTATUS_INIT_VUFORIA);
                break;
            
            case APPSTATUS_INIT_VUFORIA:
                // Initialize Vuforia SDK asynchronously to avoid blocking the
                // main (UI) thread.
                //
                // NOTE: This task instance must be created and invoked on the
                // UI thread and it can be executed only once!
                try
                {
                    mInitVuforiaTask = new InitVuforiaTask();
                    mInitVuforiaTask.execute();
                } catch (Exception e)
                {
                    Log.e("Eegeo-AR","Initializing Vuforia SDK failed");
                }
                break;
            
            case APPSTATUS_INIT_TRACKER:
                // Initialize the ObjectTracker:
                if (initTracker() > 0)
                {
                    // Proceed to next application initialization status:
                    updateApplicationStatus(APPSTATUS_INIT_APP_AR);
                }
                break;
            
            case APPSTATUS_INIT_APP_AR:
                // Initialize Augmented Reality-specific application elements
                // that may rely on the fact that the Vuforia SDK has been
                // already initialized:
                initApplicationAR();
                
                // Proceed to next application initialization status:
                updateApplicationStatus(APPSTATUS_LOAD_TRACKER);
                break;
            
            case APPSTATUS_LOAD_TRACKER:
                // Load the tracking data set:
                //
                // NOTE: This task instance must be created and invoked on the
                // UI thread and it can be executed only once!
                try
                {
                    mLoadTrackerTask = new LoadTrackerTask();
                    mLoadTrackerTask.execute();
                } catch (Exception e)
                {
                	 Log.e("Eegeo-AR","Loading tracking data set failed");
                }
                break;
            
            case APPSTATUS_INITED:
                // Hint to the virtual machine that it would be a good time to
                // run the garbage collector:
                //
                // NOTE: This is only a hint. There is no guarantee that the
                // garbage collector will actually be run.
                System.gc();
                
                // Native post initialization:
                onVuforiaInitializedNative();
                // Start the camera:
                updateApplicationStatus(APPSTATUS_CAMERA_RUNNING);
                
                break;
            
            case APPSTATUS_CAMERA_STOPPED:
                // Call the native function to stop the camera:
                stopCamera();
                break;
            
            case APPSTATUS_CAMERA_RUNNING:
                // Call the native function to start the camera:
                startCamera(mCurrentCamera);
                
                // Hides the Loading Dialog
                loadingDialogHandler.sendEmptyMessage(HIDE_LOADING_DIALOG);
                
                // Set continuous auto-focus if supported by the device,
                // otherwise default back to regular auto-focus mode.
                // This will be activated by a tap to the screen in this
                // application.
                boolean result = setFocusMode(FOCUS_MODE_CONTINUOUS_AUTO);
                if (!result)
                {
                	 Log.e("Eegeo-AR","Unable to enable continuous autofocus");
                    mContAutofocus = false;
                    setFocusMode(FOCUS_MODE_NORMAL);
                } else
                {
                    mContAutofocus = true;
                }
                
                break;
            
            default:
                throw new RuntimeException("Invalid application state");
        }
    }
    
    private native boolean setFocusMode(int mode);
    
    /** Tells native code whether we are in portait or landscape mode */
    private native void setActivityPortraitMode(boolean isPortrait);
    
    /** Initialize application GUI elements that are not related to AR. */
    private void initApplication()
    {
        setActivityPortraitMode(true);
        
        // Query display dimensions:
        storeScreenDimensions();
        
        // As long as this window is visible to the user, keep the device's
        // screen turned on and bright:
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }
    
    
    /** Native function to initialize the application. */
    private native void initApplicationNative(int width, int height);
    
    
    /** Initializes AR application components. */
    private void initApplicationAR()
    {
        // Do application initialization in native code (e.g. registering
        // callbacks, etc.):
        initApplicationNative(mScreenWidth, mScreenHeight);
        
        
        
        // Shows the loading indicator at start
        loadingDialogHandler.sendEmptyMessage(SHOW_LOADING_DIALOG);
        
    }
    
    /** Tells native code to switch dataset as soon as possible */
    private native void switchDatasetAsap(int datasetId);
    
    
    private native boolean autofocus();
    
    /** Activates the Flash */
    private native boolean activateFlash(boolean flash);
    
    
    /** Returns the number of registered textures. */
    public int getTextureCount()
    {
        return mTextures.size();
    }
    
    
    /** Returns the texture object at the specified index. */
    public Texture getTexture(int i)
    {
        return mTextures.elementAt(i);
    }
    
    /**
     * Updates projection matrix and viewport after a screen rotation change was
     * detected.
     */
    public void updateRenderView()
    {
        int currentScreenRotation = getWindowManager().getDefaultDisplay()
            .getRotation();
        if (currentScreenRotation != mLastScreenRotation)
        {
            // Set projection matrix if there is already a valid one:
            if (Vuforia.isInitialized()
                && (mAppStatus == APPSTATUS_CAMERA_RUNNING))
            {
                Log.d("EEgeo-AR","updateRenderView");
                
                // Query display dimensions:
                storeScreenDimensions();
                
                // Update viewport via renderer:
                updateRendering(mScreenWidth, mScreenHeight);
                
                // Update projection matrix:
                setProjectionMatrix();
                
                // Cache last rotation used for setting projection matrix:
                mLastScreenRotation = currentScreenRotation;
            }
        }
        renderFrame();
    }
    
	/** The native render function. */
    public native void renderFrame();
    
}