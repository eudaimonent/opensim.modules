
using System;
using System.Collections.Generic;
using System.Reflection;

using log4net;
using OpenMetaverse;
using Mono.Addins;

using OpenSim.Region.Framework.Interfaces;
using OpenSim.Region.CoreModules.World.Wind;

[assembly: Addin("SimpleFluidSolverWind", "1.0")]
[assembly: AddinDependency("OpenSim.Region.Framework", OpenSim.VersionInfo.VersionNumber)]

namespace OpenSim.Region.CoreModules.World.Wind.Plugins
{
    [Extension(Path = "/OpenSim/WindModule", NodeName = "WindModel", Id = "SimpleFluidSolverWind")]
    class SimpleFluidSolverWind : Mono.Addins.TypeExtensionNode, IWindModelPlugin
    {
        private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        private Vector2[] m_windSpeeds = new Vector2[16 * 16];
        //private Random m_rndnums = new Random(Environment.TickCount);

	 private float m_strength = 1.0f;

        private float m_avgStrength = 5.0f; // Average magnitude of the wind vector
        private float m_avgDirection = 0.0f; // Average direction of the wind in degrees
        private float m_varStrength = 5.0f; // Max Strength  Variance
        private float m_varDirection = 30.0f;// Max Direction Variance
        private float m_rateChange = 1.0f; // 

        private Vector2 m_curPredominateWind = new Vector2();


        #region IPlugin Members

        public string Version
        {
            get { return "1.0.0.0"; }
        }

        public string Name
        {
            get { return "SimpleFluidSolverWind"; }
        }

        public void Initialise()
        {
        }

        #endregion


        #region IDisposable Members

        public void Dispose()
        {
            m_windSpeeds = null;
        }

        #endregion


        #region IWindModelPlugin Members

        public void WindConfig(OpenSim.Region.Framework.Scenes.Scene scene, Nini.Config.IConfig windConfig)
        {
            if (windConfig != null)
            {
                if (windConfig.Contains("strength"))
                {
                    m_strength = windConfig.GetFloat("strength", 1.0F);
                }


                // Uses strength value if avg_strength not specified
                m_avgStrength = windConfig.GetFloat("strength", 5.0F);
                m_avgStrength = windConfig.GetFloat("avg_strength", 5.0F);

                m_avgDirection = windConfig.GetFloat("avg_direction", 0.0F);
                m_varStrength  = windConfig.GetFloat("var_strength", 5.0F);
                m_varDirection = windConfig.GetFloat("var_direction", 30.0F);
                m_rateChange   = windConfig.GetFloat("rate_change", 1.0F);

                LogSettings();
            }
        }


        public void WindUpdate(uint frame)
        {
            double avgAng = m_avgDirection * (Math.PI/180.0f);
            double varDir = m_varDirection * (Math.PI/180.0f);

            // Prevailing wind algorithm
            // Inspired by Kanker Greenacre

            // TODO: 
            // * This should probably be based on in-world time.
            // * should probably move all these local variables to class members and constants
            double time = DateTime.Now.TimeOfDay.Seconds / 86400.0f;

            double theta = time * (2 * Math.PI) * m_rateChange;

            double offset = Math.Sin(theta) * Math.Sin(theta*2) * Math.Sin(theta*9) * Math.Cos(theta*4);

            double windDir = avgAng + (varDir * offset);

            offset = Math.Sin(theta) * Math.Sin(theta*4) + (Math.Sin(theta*13) / 3);
            double windSpeed = m_avgStrength + (m_varStrength * offset);

            if (windSpeed<0) windSpeed=0;

            m_curPredominateWind.X = (float)Math.Cos(windDir);
            m_curPredominateWind.Y = (float)Math.Sin(windDir);

            m_curPredominateWind.Normalize();
            m_curPredominateWind.X *= (float)windSpeed;
            m_curPredominateWind.Y *= (float)windSpeed;

            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    m_windSpeeds[y * 16 + x] = m_curPredominateWind;
                }
            }
        }


        public Vector3 WindSpeed(float fX, float fY, float fZ)
        {
            Vector3 windVector = new Vector3(0.0f, 0.0f, 0.0f);

            int x = (int)fX / 16;
            int y = (int)fY / 16;

            if (x <  0) x = 0;
            if (x > 15) x = 15;
            if (y <  0) y = 0;
            if (y > 15) y = 15;

            if (m_windSpeeds != null)
            {
                windVector.X = m_windSpeeds[y * 16 + x].X;
                windVector.Y = m_windSpeeds[y * 16 + x].Y;
            }

            return windVector;
        }


        public Vector2[] WindLLClientArray()
        {
            return m_windSpeeds;
        }


        public string Description
        {
            get 
            {
                return "Provides a predominate wind direction that can change within configured variances for direction and speed."; 
            }
        }


        public System.Collections.Generic.Dictionary<string, string> WindParams()
        {
            Dictionary<string, string> Params = new Dictionary<string, string>();

            Params.Add("avgStrength", "average wind strength");
            Params.Add("avgDirection", "average wind direction in degrees");
            Params.Add("varStrength", "allowable variance in wind strength");
            Params.Add("varDirection", "allowable variance in wind direction in +/- degrees");
            Params.Add("rateChange", "rate of change");

            return Params;
        }


        public void WindParamSet(string param, float value)
        {
            switch (param)
            {
                case "avgStrength":
                     m_avgStrength = value;
                     break;
                case "avgDirection":
                     m_avgDirection = value;
                     break;
                 case "varStrength":
                     m_varStrength = value;
                     break;
                 case "varDirection":
                     m_varDirection = value;
                     break;
                 case "rateChange":
                     m_rateChange = value;
                     break;
            }
        }


        public float WindParamGet(string param)
        {
            switch (param)
            {
                case "avgStrength":
                    return m_avgStrength;
                case "avgDirection":
                    return m_avgDirection;
                case "varStrength":
                    return m_varStrength;
                case "varDirection":
                    return m_varDirection;
                case "rateChange":
                    return m_rateChange;
                default:
                    throw new Exception(String.Format("Unknown {0} parameter {1}", this.Name, param));

            }
        }


        #endregion


        private void LogSettings()
        {
            m_log.InfoFormat("[ConfigurableWind] Average Strength   : {0}", m_avgStrength);
            m_log.InfoFormat("[ConfigurableWind] Average Direction  : {0}", m_avgDirection);
            m_log.InfoFormat("[ConfigurableWind] Varience Strength  : {0}", m_varStrength);
            m_log.InfoFormat("[ConfigurableWind] Varience Direction : {0}", m_varDirection);
            m_log.InfoFormat("[ConfigurableWind] Rate Change        : {0}", m_rateChange);
        }

        #region IWindModelPlugin Members


        #endregion
    }
}
