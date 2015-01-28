//
// A Simple Fluid Solver Wind by Jos Stam for OpenSim
// 		http://www.dgp.utoronto.ca/people/stam/reality/Research/pub.html
//	
//  by Fumi.Iseki
//

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Security;
using System.Runtime.InteropServices;

using log4net;
using OpenMetaverse;
using Mono.Addins;

using OpenSim.Framework;
using OpenSim.Region.Framework.Interfaces;
using OpenSim.Region.CoreModules.World.Wind;


[assembly: Addin("SimpleFluidSolverWind", "1.0")]
[assembly: AddinDependency("OpenSim.Region.Framework", OpenSim.VersionInfo.VersionNumber)]


namespace OpenSim.Region.CoreModules.World.Wind.Plugins
{
	[Extension(Path = "/OpenSim/WindModule", NodeName = "WindModel", Id = "SimpleFluidSolverWind")]
	class SimpleFluidSolverWind : Mono.Addins.TypeExtensionNode, IWindModelPlugin
	{
		private const int   m_mesh  = 16;
		private int   m_init_force  = 0;
		private int   m_region_size = 256;
		private float m_damping_rate = 0.85f;

		private static readonly ILog m_log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

		private Vector2[] m_windSpeeds = new Vector2[m_mesh*m_mesh];

		private float m_strength = 1.0f;
		private Random m_rndnums = new Random(Environment.TickCount);

		private float[] m_windSpeeds_u = new float[m_mesh*m_mesh];
		private float[] m_windSpeeds_v = new float[m_mesh*m_mesh];
		private float[] m_windForces_u = new float[m_mesh*m_mesh];
		private float[] m_windForces_v = new float[m_mesh*m_mesh];
		private float[] m_initForces_u = new float[m_mesh*m_mesh];
		private float[] m_initForces_v = new float[m_mesh*m_mesh];

		//
		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private extern static void init_SFSW(int n);

		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private extern static void free_SFSW();

		[DllImport("sfsw", CallingConvention = CallingConvention.Cdecl), SuppressUnmanagedCodeSecurity]
		private static extern void solve_SFSW(int n, [MarshalAs(UnmanagedType.LPArray)] float[] u,  [MarshalAs(UnmanagedType.LPArray)] float[] v, 
													 [MarshalAs(UnmanagedType.LPArray)] float[] u0, [MarshalAs(UnmanagedType.LPArray)] float[] v0,
													 int rsize, float visc, float dt);

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
			init_SFSW(m_mesh);

			for (int i=0; i<m_mesh*m_mesh; i++)
			{
				m_windSpeeds_u[i] = 0.0f;
				m_windSpeeds_v[i] = 0.0f;
			}
			//
			initForces();
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
			m_initForces_u = null;
			m_initForces_v = null;

			free_SFSW();
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
				if (windConfig.Contains("damping"))
				{
					m_damping_rate = windConfig.GetFloat("damping", 0);
					if (m_damping_rate>1.0f) m_damping_rate = 1.0f;
				}
				if (windConfig.Contains("force"))
				{
					m_init_force = windConfig.GetInt("force", 0);
					if (m_init_force<0 || m_init_force>2) m_init_force = 0;
				}
				//
				if (windConfig.Contains("region"))
				{
					m_region_size = windConfig.GetInt("region", 256);
				    m_region_size = (((int)Math.Abs(m_region_size)+255)/256)*256;
					if (m_region_size==0) m_region_size = 256;
				}
				if (scene!=null)
				{
					m_region_size = (int)scene.RegionInfo.RegionSizeX;
				}
			}
		}


		public void WindUpdate(uint frame)
		{
			if (m_windSpeeds!=null)
			{
				for (int i=0; i<m_mesh*m_mesh; i++) {
					m_windForces_u[i] = m_initForces_u[i]*m_strength;
					m_windForces_v[i] = m_initForces_v[i]*m_strength;
				}

				solve_SFSW(m_mesh, m_windSpeeds_u, m_windSpeeds_v, m_windForces_u, m_windForces_v, m_region_size, 0.001f, 1.0f);
				//m_log.InfoFormat("[SimpleFluidSolverWind] ZeroPt Strength : {0} {1}", m_windSpeeds_u[0], m_windSpeeds_v[0]);
				//m_log.InfoFormat("[SimpleFluidSolverWind] Center Strength : {0} {1}", m_windSpeeds_u[m_mesh*m_mesh/2], m_windSpeeds_v[m_mesh*m_mesh/2]);
				//
				for (int i=0; i<m_mesh*m_mesh; i++)
				{
					m_windSpeeds[i].X = m_windSpeeds_u[i];
					m_windSpeeds[i].Y = m_windSpeeds_v[i];
					//
					m_initForces_u[i] *= m_damping_rate;
					m_initForces_v[i] *= m_damping_rate;
				}
			}
		}


		public Vector3 WindSpeed(float fX, float fY, float fZ)
		{
			Vector3 windVector = new Vector3(0.0f, 0.0f, 0.0f);

			int x = (int)fX/m_mesh;
			int y = (int)fY/m_mesh;

			if (x<0) 		x = 0;
			if (x>m_mesh-1) x = m_mesh - 1;
			if (y<0) 		y = 0;
			if (y>m_mesh-1) y = m_mesh - 1;

			if (m_windSpeeds!=null)
			{
				windVector.X = m_windSpeeds[y*m_mesh + x].X;
				windVector.Y = m_windSpeeds[y*m_mesh + x].Y;
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
			Params.Add("force", "initial force");
			Params.Add("damping", "damping of force");
			Params.Add("region", "size of region");
			return Params;
		}


		public void WindParamSet(string param, float value)
		{
			switch(param)
			{
				case "strength":
				  m_strength = value;
				  m_log.InfoFormat("[SimpleFluidSolverWind] Set Param : strength = {0}", m_strength);
				  break;

				case "damping":
				  m_damping_rate = value;
				  if (m_damping_rate>1.0f) m_damping_rate = 1.0f;
				  m_log.InfoFormat("[SimpleFluidSolverWind] Set Param : damping = {0}", m_damping_rate);
				  break;

				case "force":
				  m_init_force = (int)value;
				  if (m_init_force<0 || m_init_force>2) m_init_force = 0;
				  m_log.InfoFormat("[SimpleFluidSolverWind] Set Param : force = {0}", m_init_force);
				  Initialise();
				  break;

				case "region":
				  m_region_size = (((int)Math.Abs(value)+255)/256)*256;
				  if (m_region_size==0) m_region_size = 256;
				  m_log.InfoFormat("[SimpleFluidSolverWind] Set Param : region = {0}", m_region_size);
				  break;
			}
		}


		public float WindParamGet(string param)
		{
			switch (param)
			{
				case "strength":
				  return m_strength;

				case "damping":
				  return m_damping_rate;

				case "force":
				  return (float)m_init_force;

				case "region":
				  return (float)m_region_size;

				default:
				  throw new Exception(String.Format("Unknown {0} parameter {1}", this.Name, param));
			}
		}


		#endregion


		//
		public void initForces()
		{
			for (int i=0; i<m_mesh*m_mesh; i++)
			{
				m_initForces_u[i] = 0.0f; 
				m_initForces_v[i] = 0.0f; 
			}
			//
			setInitForces(m_init_force);
		}


		private void setInitForces(int init)
		{
			int i, j;

			if (init==0)
			{
				for (i=0; i<m_mesh*m_mesh; i++) {
					m_initForces_u[i] = (float)(m_rndnums.NextDouble()*2d - 1d); // -1 to 1 
					m_initForces_v[i] = (float)(m_rndnums.NextDouble()*2d - 1d); // -1 to 1 
				}
			}

			else if (init==1) 
			{
				for (i=0, j=m_mesh-1; i<m_mesh/3; i++, j--) {
					m_initForces_u[i + j*m_mesh] += 1.0f + ((float)i) * 0.1f;
					m_initForces_v[i + j*m_mesh] -= 1.0f + ((float)i) * 0.1f;
				}
			}

			else if (init==2) 
			{
				for (j=m_mesh/10; j<m_mesh-m_mesh/10; j++) {
					i = m_mesh/10;
					m_initForces_v[i + j*m_mesh] += 0.1f;
					i = m_mesh - m_mesh/10;
					m_initForces_v[i + j*m_mesh] -= 0.1f;
				}
	
				for (i=m_mesh/10; i<m_mesh-m_mesh/10; i++) {
					j = m_mesh/10;
					m_initForces_u[i + j*m_mesh] -= 0.1f;
					j = m_mesh-m_mesh/10;
					m_initForces_u[i + j*m_mesh] += 0.1f;
				}
	
				float radius = ((float) m_mesh)/4.0f;

				for (float f=0.0f; f<360.0; f+=0.25f) {
					float angle = f/(2*(float)Math.PI);
					float x = (float)Math.Sin(angle)*radius;
					float y = (float)Math.Cos(angle)*radius;
					//
					m_initForces_u[(m_mesh/2+(int)Math.Floor(x)) + (m_mesh/2+(int)Math.Floor(y))*m_mesh] -= (float)Math.Cos(angle)*0.1f;
					m_initForces_v[(m_mesh/2+(int)Math.Floor(x)) + (m_mesh/2+(int)Math.Floor(y))*m_mesh] += (float)Math.Sin(angle)*0.1f;	  
				}	
			}
		}



/*
		private void LogSettings()
		{
			m_log.InfoFormat("[SimpleFluidSolverWind] Average Strength : {0}", m_strength);
		}
*/

	}
}
