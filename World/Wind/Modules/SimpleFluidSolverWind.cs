
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Security;
using System.Runtime.InteropServices;

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

		private Vector2[] m_windSpeeds = new Vector2[16*16];

		private float m_strength = 1.0f;
		private Random m_rndnums = new Random(Environment.TickCount);

		private float[] m_windSpeeds_u = new float[16*16];
		private float[] m_windSpeeds_v = new float[16*16];
		private float[] m_windForces_u = new float[16*16];
		private float[] m_windForces_v = new float[16*16];

		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private extern static void init_FFT(int n);

		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private extern static void close_FFT();

		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private static extern void stable_solve(int n, [MarshalAs(UnmanagedType.LPArray)] float[] u,  [MarshalAs(UnmanagedType.LPArray)] float[] v, 
													   [MarshalAs(UnmanagedType.LPArray)] float[] u0, [MarshalAs(UnmanagedType.LPArray)] float[] v0, float visc, float dt);

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
			init_FFT(16);

			for (int i=0; i<16*16; i++)
			{
				m_windSpeeds_u[i] = 0.0f;
				m_windSpeeds_v[i] = 0.0f;
				m_windForces_u[i] = 0.0f;
				m_windForces_v[i] = 0.0f;
			}
		}

		#endregion


		#region IDisposable Members

		public void Dispose()
		{
			m_windSpeeds   = null;
			//
			m_windSpeeds_u = null;
			m_windSpeeds_v = null;
			m_windForces_u = null;
			m_windForces_v = null;

			close_FFT();
		}

		#endregion


		#region IWindModelPlugin Members

		public void WindConfig(OpenSim.Region.Framework.Scenes.Scene scene, Nini.Config.IConfig windConfig)
		{
			if (windConfig != null)
			{
				if (windConfig.Contains("strength"))
				{
					m_strength = windConfig.GetFloat("strength", 1.0f);
				}
			}
		}


		public void WindUpdate(uint frame)
		{
			if (m_windSpeeds!=null) {
				for (int y=0; y<16; y++)
				{
					for (int x=0; x<16; x++)
					{
						m_windForces_u[y*16 + x] = (float)(m_rndnums.NextDouble()*2d - 1d); // -1 to 1
						m_windForces_v[y*16 + x] = (float)(m_rndnums.NextDouble()*2d - 1d); // -1 to 1
						m_windForces_u[y*16 + x] *= m_strength;
						m_windForces_v[y*16 + x] *= m_strength;
					}
				}
			
				stable_solve(16, m_windSpeeds_u, m_windSpeeds_v, m_windForces_u, m_windForces_v, 0.001f, 1.0f);
				//m_log.InfoFormat("[SimpleFluidSolverWind] Average Strength : {0} {1}", m_windSpeeds_u[0], m_windSpeeds_v[0]);

				for (int i=0; i<16*16; i++)
				{
					m_windSpeeds[i].X = m_windSpeeds_u[i];
					m_windSpeeds[i].Y = m_windSpeeds_v[i];
				}
			}
		}


		public Vector3 WindSpeed(float fX, float fY, float fZ)
		{
			Vector3 windVector = new Vector3(0.0f, 0.0f, 0.0f);

			int x = (int)fX/16;
			int y = (int)fY/16;

			if (x< 0) x = 0;
			if (x>15) x = 15;
			if (y< 0) y = 0;
			if (y>15) y = 15;

			if (m_windSpeeds!=null)
			{
				windVector.X = m_windSpeeds[y*16 + x].X;
				windVector.Y = m_windSpeeds[y*16 + x].Y;
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
				return "Provides a simple fluid solver wind by Jos Stam."; 
			}
		}


		public System.Collections.Generic.Dictionary<string, string> WindParams()
		{
			Dictionary<string, string> Params = new Dictionary<string, string>();

			Params.Add("strength", "wind strength");
			return Params;
		}


		public void WindParamSet(string param, float value)
		{
			switch(param)
			{
				case "strength":
				  m_strength = value;
				  break;
			}
		}


		public float WindParamGet(string param)
		{
			switch (param)
			{
				case "strength":
				  return m_strength;

				default:
				  throw new Exception(String.Format("Unknown {0} parameter {1}", this.Name, param));
			}
		}


		#endregion


/*
		private void LogSettings()
		{
			m_log.InfoFormat("[SimpleFluidSolverWind] Average Strength : {0}", m_strength);
		}
*/

	}
}
