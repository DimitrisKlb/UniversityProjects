/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.awt.*;
import java.util.Arrays;
import javax.swing.JFrame;
import java.awt.image.BufferStrategy;
import java.awt.image.BufferedImage;

public final class Engine extends JFrame
{
   private static Shape3D       Shapes[];
       private static Light         Lights[];
   private static Thread        Animator;

   private static BufferedImage RenderImage;
   private static Graphics2D    RenderGraphics2D;

   public static void main(String[] args)
   {
      for (int i = 0; i < args.length; i++)
	  {
		 if (args[i].toUpperCase().endsWith(".S3O") == true)
		 {
		    new Engine(args[i]);
            return;
         }
	  }
	  
	  System.out.println("No .s3o file in input params!");
   }

   public Engine(final String Filename)
   {
      setTitle("3D Engine Test");
      setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      setSize(ScreenTrans.SCREEN_WIDTH, ScreenTrans.SCREEN_HEIGHT);
      setResizable(false);
      setVisible(true);
      createBufferStrategy(2);
      setIgnoreRepaint(true);
	
      RenderImage      = new BufferedImage(ScreenTrans.SCREEN_WIDTH, ScreenTrans.SCREEN_HEIGHT,
                                           BufferedImage.TYPE_INT_RGB);
      RenderGraphics2D = (Graphics2D)RenderImage.getGraphics();

      if (LoadObjects(Filename) == true)
         RenderLoop();
   }

   private boolean LoadObjects(final String Filename)
   {
      System.out.println("\n\n\n\n\nLoading graphics objects...");

      /* OBJECT CREATION */
      Shapes = new Shape3D[1];

      Shapes[0] = S3OLoader.LoadS3OFile(Filename);
      /* END OBJECT CREATION*/

      for (int i = (Shapes.length - 1); i >= 0 ; --i)
      {
         if (Shapes[i] == null)
         {
            System.out.println("One or more shapes null, aborting...");
            return false;
         }
      }

      System.out.println("Loaded graphics objects.\n");

      System.out.println("Locating objects in 3D space...");

      /* OBJECT LOCATING */
      Shapes[0].SetLocation(new Point(10, 10, -40));
      /* END OBJECT LOCATING */            

      System.out.println("Objects located.\n");

      System.out.println("Calculating totals...");

      long TotalPoints    = 0;
      long TotalVerticies = 0;

      for (int i = (Shapes.length - 1); i >= 0 ; --i)
      {
         TotalPoints    += Shapes[i].GetPoints().length;
         TotalVerticies += Shapes[i].GetTriangles().length;
      }

      System.out.println("  @ Total Objects:  " + Shapes.length +
            "\n  @ Total Points:   " + TotalPoints   +
            "\n  @ Total Faces:    " + TotalVerticies);

      System.out.println("Totals calculated.\n");

      System.out.println("Creating lights...");

      /* LIGHT CREATION */
      Lights = new Light[3];

      Lights[0] = new Light(new Point(  20,   20, -100), 1.0f, new Color(255, 255, 255), 5);
      Lights[1] = new Light(new Point(  20,   50,    0), 1.0f, new Color(  0,   0, 100));
      Lights[2] = new Light(new Point(   5,   5 ,   20), 0.3f, new Color( 50,   0,   0));
      /* END LIGHT CREATION */

      System.out.println("  Ambient Light:"
            + "\n     # Intensity: " + (ScreenTrans.SCREEN_AMBIENT_INT * 100) + "%"
            + "\n     # Color:     R: " + ScreenTrans.SCREEN_AMBIENT.getRed()
            +                   "  G: " + ScreenTrans.SCREEN_AMBIENT.getGreen()
            +                   "  B: " + ScreenTrans.SCREEN_AMBIENT.getBlue()
            + "\n\n  @ Total Lights:  " + Lights.length);

      System.out.println("Lights created.\n");

      System.out.println("Coloring objects...");

      Color[] ColorList = new Color[(int)TotalVerticies];

      /*
         for (int i = (ColorList.length - 1); i >= 0 ; --i)
         {
            ColorList[i] = new Color((int)(Math.random() * 255),
                                     (int)(Math.random() * 255),
                                     (int)(Math.random() * 255));
         }

         for (int i = (Shapes.length - 1); i >= 0 ; --i)
         {
            Shapes[i].ColorShape(ColorList);
         }
       */

      System.out.println("Objects colored.\n");
	  
	  return true;
   }

   private void RenderLoop()
   {
      BufferStrategy Strategy = getBufferStrategy();

      final double   RotXDegF  = ((Math.PI / 180) * 2);
      final double   RotYDegF  = ((Math.PI / 180) * 1);

      final float    RotXSin   = (float)Math.sin(RotXDegF);
      final float    RotXCos   = (float)Math.cos(RotXDegF);
      final float    RotYSin   = (float)Math.sin(RotYDegF);
      final float    RotYCos   = (float)Math.cos(RotYDegF);

      final float    RotLXSin  = (float)Math.sin(-2 * RotXDegF);
      final float    RotLXCos  = (float)Math.cos(-2 * RotXDegF);
      final float    RotLYSin  = (float)Math.sin(3 * RotYDegF);
      final float    RotLYCos  = (float)Math.cos(3 * RotYDegF);

      long FPSTimer         = System.currentTimeMillis();
      long FrameStartTime;

      int  FramesPerSec     = 0;

      System.out.println("\n\n");

      for(;;)
      {
         FrameStartTime = System.currentTimeMillis();
         
         if (FrameStartTime >= (FPSTimer + 1000))
         {
            FPSTimer = FrameStartTime;

            System.out.print("\rFrames per second: " + FramesPerSec + "    ");

            FramesPerSec = 0;
         }

         FramesPerSec++;

         for (int i = (Shapes.length - 1); i >= 0 ; --i)
         {
            Shapes[i].RotatePointsX(RotXSin, RotXCos);
            Shapes[i].RotatePointsY(RotYSin, RotYCos);
         }

         Lights[0].GetLocation().RotateX(RotLXSin, RotLXCos);
         Lights[0].GetLocation().RotateY(RotLYSin, RotLYCos);
         Lights[1].GetLocation().RotateX(RotLXSin, RotLXCos);

         Arrays.sort(Shapes);

         RenderGraphics2D.setColor(ScreenTrans.SCREEN_BGCOLOR);
         RenderGraphics2D.fillRect(0, 0, ScreenTrans.SCREEN_WIDTH, ScreenTrans.SCREEN_HEIGHT);

         for (int i = (Shapes.length - 1); i >= 0 ; --i)
         {
            Shapes[i].DrawShape(RenderGraphics2D, Lights);
         }

         LightManager.DrawLights(RenderGraphics2D, Lights);

         Graphics Screen = Strategy.getDrawGraphics();

         try
         {
            Screen.drawImage(RenderImage, 0, 0, null);
         }
         finally
         {
            Screen.dispose();
         }

         Strategy.show();

         int FrameRenderTime = (int)(System.currentTimeMillis() - FrameStartTime);

         if(FrameRenderTime < 33)
         { 
            try
            {
               Thread.sleep(33 - FrameRenderTime);
            }
            catch (Exception e)
            {
               return;
            }
         }
      }
   }
}
