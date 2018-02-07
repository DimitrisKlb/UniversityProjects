/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.awt.Color;
import java.awt.Graphics2D;
import java.util.Arrays;

public final class LightManager
{
   static float[] TrianglePos    = new float[3];
   static float[] TriangleRGB    = new float[3];
   static float[] RGBIntensities = new float[3];

   static public Color LightTriangle(final Light[] Lights, final Point[] TriPoints, final Color TriColor)
   {
      return LightTriangle(Lights, Point.ORIGIN, TriPoints, TriColor);
   }

   static public Color LightTriangle(final Light[] Lights, final Point TriLocation, final Point[] TriPoints, final Color TriColor)
   {
      float[] TrianglePosL    = TrianglePos;
      float[] TriangleRGBL    = TriangleRGB;
      float[] RGBIntensitiesL = RGBIntensities;

      ScreenTrans.SCREEN_AMBIENT.getRGBColorComponents(RGBIntensitiesL);
      RGBIntensitiesL[0] *= ScreenTrans.SCREEN_AMBIENT_INT;
      RGBIntensitiesL[1] *= ScreenTrans.SCREEN_AMBIENT_INT;
      RGBIntensitiesL[2] *= ScreenTrans.SCREEN_AMBIENT_INT;

      Arrays.fill(TrianglePosL, 0);

      for (int i = 2; i >= 0; --i)
      {
         TrianglePosL[0] += TriPoints[i].GetX();
         TrianglePosL[1] += TriPoints[i].GetY();
         TrianglePosL[2] += TriPoints[i].GetZ();
      }

      TrianglePosL[0] = (TrianglePosL[0] / 3) + TriLocation.GetX();
      TrianglePosL[1] = (TrianglePosL[1] / 3) + TriLocation.GetY();
      TrianglePosL[2] = (TrianglePosL[2] / 3) + TriLocation.GetZ();

      for (int i = (Lights.length - 1); i >= 0; --i)
      {
         Lights[i].ApplyLight(TriPoints, TrianglePosL, RGBIntensitiesL);
      }

      TriColor.getRGBColorComponents(TriangleRGBL);
      TriangleRGBL[0] *= RGBIntensitiesL[0];
      TriangleRGBL[1] *= RGBIntensitiesL[1];
      TriangleRGBL[2] *= RGBIntensitiesL[2];

      return new Color(((TriangleRGBL[0] > 1) ? 1 : TriangleRGBL[0]),
                       ((TriangleRGBL[1] > 1) ? 1 : TriangleRGBL[1]),
                       ((TriangleRGBL[2] > 1) ? 1 : TriangleRGBL[2]));
   }

   static public void DrawLights(final Graphics2D Screen, final Light[] Lights)
   {
      float ZScale;
      float Lx, Ly;
      int   S1x, S1y, S2x, S2y;
      Point LightLocation;

      for (int i = (Lights.length - 1); i >= 0; --i)
      {
         LightLocation = Lights[i].GetLocation();
         ZScale = (ScreenTrans.FOCAL / (ScreenTrans.FOCAL + LightLocation.GetZ()));

         Lx  = LightLocation.GetX();
         Ly  = LightLocation.GetY();

         S1x = ((int)((Lx - 5) * ZScale) + ScreenTrans.VP_X);
         S1y = ((int)((Ly - 5) * ZScale) + ScreenTrans.VP_Y);

         S2x = ((int)((Lx + 5) * ZScale) + ScreenTrans.VP_X);
         S2y = ((int)((Ly + 5) * ZScale) + ScreenTrans.VP_Y);

         Screen.setColor(Lights[i].GetColor());
         Screen.drawLine(S1x, S1y, S2x, S2y);
         Screen.drawLine(S2x, S1y, S1x, S2y);
      }   
   }
}

