/*
          Simple Software 3D Java Rendering Engine
                  (C) Dean Camera, 2007

        dean_camera (at} fourwalledcubicle [dot> com
             http://www.fourwalledcubicle.com
*/

import java.awt.Color;

public final class Light
{
   private Point   Position;
   private float   Intensity;
   private Color   LColor;
   private float   DropOff;
   private float   DropOffSq;

   private float[] LightRGB;

   public Light(final float Intensity, final Color LColor, final float DropOff)
   {
      this(Point.ORIGIN, Intensity, LColor, DropOff);
   }

   public Light(final float Intensity, final Color LColor)
   {
      this(Point.ORIGIN, Intensity, LColor, 0);
   }

   public Light(final Point Position, final float Intensity, final Color LColor)
   {
      this(Position, Intensity, LColor, 0);
   }
   
   public Light(final Point Position, final float Intensity, final Color LColor, final float DropOff)
   {
      this.Position  = Position;
      this.Intensity = Intensity;
      this.LColor    = LColor;
      this.DropOff   = DropOff;
      this.DropOffSq = (DropOff * DropOff);

      LightRGB = new float[3];
   }

   public Point GetLocation()
   {
      return Position;
   }

   public Color GetColor()
   {
      return LColor;
   }
   
   public float GetDropOff()
   {
      return DropOff;
   }
   
   public void SetDropOff(final float DropOff)
   {
      this.DropOff   = DropOff;
      this.DropOffSq = (DropOff * DropOff);
   }
   
   public float GetIntensity()
   {
      return Intensity;
   }
   
   public void SetIntensity(final float Intensity)
   {
      this.Intensity = Intensity;
   }

   public void ApplyLight(final Point[] TriPoints, final float[] TriLocations, final float[] RGBIntensities)
   {
      float[] LightRGB = this.LightRGB;

      float T1x, T1y, T1z;
      float T2x, T2y, T2z;
      float Nx,  Ny,  Nz;
      float DotProd, NormMag, LightMag;
      float AngIntensity;
      float TriPoints1X, TriPoints1Y, TriPoints1Z;
      float PosX, PosY, PosZ;
      float FaceIntensity;
      float TDistSq;

      TriPoints1X = TriPoints[1].GetX();
      TriPoints1Y = TriPoints[1].GetY();
      TriPoints1Z = TriPoints[1].GetZ();
 
      T1x      = (TriPoints[0].GetX() - TriPoints1X);
      T1y      = (TriPoints[0].GetY() - TriPoints1Y);
      T1z      = (TriPoints[0].GetZ() - TriPoints1Z);
     
      T2x      = (TriPoints1X - TriPoints[2].GetX());
      T2y      = (TriPoints1Y - TriPoints[2].GetY());
      T2z      = (TriPoints1Z - TriPoints[2].GetZ());

      Nx       = ((T1y * T2z) - (T1z * T2y)); 
      Ny       = ((T1z * T2x) - (T1x * T2z));
      Nz       = ((T1x * T2y) - (T1y * T2x)); 

      PosX     = Position.GetX();
      PosY     = Position.GetY();
      PosZ     = Position.GetZ();

      DotProd  = ((Nx * PosX) + (Ny * PosY) + (Nz * PosZ));
      NormMag  = ((Nx * Nx) + (Ny * Ny) + (Nz * Nz));
      LightMag = ((PosX * PosX)
               +  (PosY * PosY)
               +  (PosZ * PosZ));

      FaceIntensity = (float)(1 - (Math.acos(DotProd / Math.sqrt(NormMag * LightMag)) / Math.PI));

      if (DropOff > 0)
      {
         TDistSq = (Math.abs(TriLocations[0] - PosX)
                 +  Math.abs(TriLocations[1] - PosY)
                 +  Math.abs(TriLocations[2] - PosZ));

         if (TDistSq > DropOffSq)
            FaceIntensity *= (DropOff / Math.sqrt(TDistSq));
      }
  
      FaceIntensity *= Intensity;

      LColor.getRGBColorComponents(LightRGB);
      RGBIntensities[0] += (LightRGB[0] * FaceIntensity);
      RGBIntensities[1] += (LightRGB[1] * FaceIntensity);
      RGBIntensities[2] += (LightRGB[2] * FaceIntensity);
   }
}

