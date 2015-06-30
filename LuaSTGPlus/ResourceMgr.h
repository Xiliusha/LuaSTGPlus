#pragma once
#include "Global.h"
#include "ObjectPool.hpp"
#include "Dictionary.hpp"

#ifdef LoadImage
#undef LoadImage
#endif

namespace LuaSTGPlus
{
	class ResourceMgr;

	/// @brief 资源类型
	enum class ResourceType
	{
		Texture = 1,
		Sprite,
		Animation,
		Music,
		SoundEffect,
		Particle,
		SpriteFont,
		TrueTypeFont
	};

	/// @brief 资源池类型
	enum class ResourcePoolType
	{
		None,
		Global,
		Stage
	};

	/// @brief 混合模式
	enum class BlendMode
	{
		AddAdd = 1,
		AddAlpha,
		MulAdd,
		MulAlpha
	};
	
	/// @brief 资源接口
	class Resource :
		public fcyRefObjImpl<fcyRefObj>
	{
	private:
		ResourceType m_Type;
		std::string m_ResName;
	public:
		ResourceType GetType()const LNOEXCEPT { return m_Type; }
		const std::string& GetResName()const LNOEXCEPT { return m_ResName; }
	private:
		Resource& operator=(const Resource&);
		Resource(const Resource&);
	public:
		Resource(ResourceType t, const char* name)
			: m_Type(t), m_ResName(name) {}
	};

	/// @brief 纹理资源
	class ResTexture :
		public Resource
	{
	private:
		fcyRefPointer<f2dTexture2D> m_Texture;
	public:
		f2dTexture2D* GetTexture() { return m_Texture; }
	public:
		ResTexture(const char* name, fcyRefPointer<f2dTexture2D> tex)
			: Resource(ResourceType::Texture, name), m_Texture(tex) {}
	};

	/// @brief 图像资源
	class ResSprite :
		public Resource
	{
	private:
		fcyRefPointer<f2dSprite> m_Sprite;
		BlendMode m_BlendMode = BlendMode::MulAlpha;
		double m_HalfSizeX = 0.;
		double m_HalfSizeY = 0.;
		bool m_bRectangle = false;
	public:
		f2dSprite* GetSprite()LNOEXCEPT { return m_Sprite; }
		BlendMode GetBlendMode()const LNOEXCEPT { return m_BlendMode; }
		void SetBlendMode(BlendMode m)LNOEXCEPT { m_BlendMode = m; }
		double GetHalfSizeX()const LNOEXCEPT { return m_HalfSizeX; }
		double GetHalfSizeY()const LNOEXCEPT { return m_HalfSizeY; }
		bool IsRectangle()const LNOEXCEPT { return m_bRectangle; }
	public:
		ResSprite(const char* name, fcyRefPointer<f2dSprite> sprite, double hx, double hy, bool rect)
			: Resource(ResourceType::Sprite, name), m_Sprite(sprite), m_HalfSizeX(hx), m_HalfSizeY(hy), m_bRectangle(rect)
		{
			m_Sprite->SetColor(0xFFFFFFFF);  // 适应乘法
		}
	};

	/// @brief 动画资源
	class ResAnimation :
		public Resource
	{
	private:
		std::vector<fcyRefPointer<f2dSprite>> m_ImageSequences;
		fuInt m_Interval = 1;
		BlendMode m_BlendMode = BlendMode::MulAlpha;
		double m_HalfSizeX = 0.;
		double m_HalfSizeY = 0.;
		bool m_bRectangle = false;
	public:
		size_t GetCount()const LNOEXCEPT { return m_ImageSequences.size(); }
		f2dSprite* GetSprite(fuInt index)LNOEXCEPT
		{
			if (index >= GetCount())
				return nullptr;
			return m_ImageSequences[index];
		}
		fuInt GetInterval()const LNOEXCEPT { return m_Interval; }
		BlendMode GetBlendMode()const LNOEXCEPT { return m_BlendMode; }
		void SetBlendMode(BlendMode m)LNOEXCEPT { m_BlendMode = m; }
		double GetHalfSizeX()const LNOEXCEPT { return m_HalfSizeX; }
		double GetHalfSizeY()const LNOEXCEPT { return m_HalfSizeY; }
		bool IsRectangle()const LNOEXCEPT { return m_bRectangle; }
	public:
		ResAnimation(const char* name, fcyRefPointer<ResTexture> tex, float x, float y, float w, float h,
			int n, int m, int intv, double a, double b, bool rect = false);
	};

	/// @brief 粒子系统
	/// @note HGE粒子系统的f2d实现
	class ResParticle :
		public Resource
	{
	public:
		/// @brief 粒子信息
		struct ParticleInfo
		{
			fuInt iBlendInfo;

			int nEmission;  // 每秒发射个数
			float fLifetime;  // 生命期
			float fParticleLifeMin;  // 粒子最小生命期
			float fParticleLifeMax;  // 粒子最大生命期
			float fDirection;  // 发射方向
			float fSpread;  // 偏移角度
			bool bRelative;  // 使用相对值还是绝对值

			float fSpeedMin;  // 速度最小值
			float fSpeedMax;  // 速度最大值

			float fGravityMin;  // 重力最小值
			float fGravityMax;  // 重力最大值

			float fRadialAccelMin;  // 最低线加速度
			float fRadialAccelMax;  // 最高线加速度

			float fTangentialAccelMin;  // 最低角加速度
			float fTangentialAccelMax;  // 最高角加速度

			float fSizeStart;  // 起始大小
			float fSizeEnd;  // 最终大小
			float fSizeVar;  // 大小抖动值

			float fSpinStart;  // 起始自旋
			float fSpinEnd;  // 最终自旋
			float fSpinVar;  // 自旋抖动值

			float colColorStart[4];  // 起始颜色(rgba)
			float colColorEnd[4];  // 最终颜色
			float fColorVar;  // 颜色抖动值
			float fAlphaVar;  // alpha抖动值
		};
		/// @brief 粒子实例
		struct ParticleInstance
		{
			fcyVec2 vecLocation;  // 位置
			fcyVec2 vecVelocity;  // 速度

			float fGravity;  // 重力
			float fRadialAccel;  // 线加速度
			float fTangentialAccel;  // 角加速度

			float fSpin;  // 自旋
			float fSpinDelta;  // 自旋增量

			float fSize;  // 大小
			float fSizeDelta;  // 大小增量

			float colColor[4];  // 颜色
			float colColorDelta[4];  // 颜色增量

			float fAge;  // 当前存活时间
			float fTerminalAge;  // 终止时间
		};
		/// @brief 粒子池
		class ParticlePool
		{
			friend class ResParticle;
		public:
			enum class Status
			{
				Alive,
				Sleep
			};
		private:
			size_t m_iId;  // 对象id
			fcyRefPointer<ResParticle> m_pInstance;  // 信息

			BlendMode m_BlendMode = BlendMode::MulAlpha;
			Status m_iStatus = Status::Alive;  // 状态
			fcyVec2 m_vCenter;  // 中心
			fcyVec2 m_vPrevCenter;  // 上一个中心
			float m_fRotation = 0.f;  // 方向
			size_t m_iAlive = 0;  // 存活数
			float m_fAge = 0.f;  // 已存活时间
			float m_fEmission = 0.f;  // 每秒发射数
			float m_fEmissionResidue = 0.f;  // 不足的粒子数
			std::array<ParticleInstance, LPARTICLE_MAXCNT> m_ParticlePool;
		public:
			size_t GetAliveCount()const LNOEXCEPT { return m_iAlive; }
			BlendMode GetBlendMode()const LNOEXCEPT { return m_BlendMode; }
			void SetBlendMode(BlendMode m)LNOEXCEPT { m_BlendMode = m; }
			float GetEmission()const LNOEXCEPT { return m_fEmission; }
			void SetEmission(float e)LNOEXCEPT { m_fEmission = e; }
			void SetActive()LNOEXCEPT
			{
				m_iStatus = Status::Alive;
				m_fAge = 0.f;
				m_fEmissionResidue = 0.f;
			}
			void SetInactive()LNOEXCEPT
			{
				m_iStatus = Status::Sleep;
			}
			void SetCenter(fcyVec2 pos)LNOEXCEPT
			{
				if (m_iStatus == Status::Alive)
					m_vPrevCenter = m_vCenter;
				else
					m_vPrevCenter = pos;
				m_vCenter = pos;
			}
			void SetRotation(float r)LNOEXCEPT { m_fRotation = r; }
			void Update(float delta);
			void Render(f2dGraphics2D* graph, float scaleX, float scaleY);
		public:
			ParticlePool(size_t id, fcyRefPointer<ResParticle> ref);
		};
	private:
		struct PARTICLE_POD { char buf[sizeof(ParticlePool)]; };
		static FixedObjectPool<PARTICLE_POD, LPARTICLESYS_MAX> s_MemoryPool;

		fcyRefPointer<f2dSprite> m_BindedSprite;
		BlendMode m_BlendMode = BlendMode::MulAlpha;
		ParticleInfo m_ParticleInfo;
		double m_HalfSizeX = 0.;
		double m_HalfSizeY = 0.;
		bool m_bRectangle = false;
	public:
		ParticlePool* AllocInstance()LNOEXCEPT;
		void FreeInstance(ParticlePool* p)LNOEXCEPT;

		f2dSprite* GetBindedSprite()LNOEXCEPT { return m_BindedSprite; }
		const ParticleInfo& GetParticleInfo()const LNOEXCEPT { return m_ParticleInfo; }
		double GetHalfSizeX()const LNOEXCEPT { return m_HalfSizeX; }
		double GetHalfSizeY()const LNOEXCEPT { return m_HalfSizeY; }
		bool IsRectangle()const LNOEXCEPT { return m_bRectangle; }
	public:
		ResParticle(const char* name, const ParticleInfo& pinfo, fcyRefPointer<f2dSprite> sprite, BlendMode bld, double a, double b, bool rect = false);
	};

	/// @brief 纹理字体
	class ResFont :
		public Resource
	{
	public:
		enum class FontAlignHorizontal  // 水平对齐
		{
			Left,
			Center,
			Right
		};
		enum class FontAlignVertical  // 垂直对齐
		{
			Top,
			Middle,
			Bottom
		};

		class HGEFont :
			public fcyRefObjImpl<f2dFontProvider>
		{
		public:
			static void ReadDefine(const std::wstring& data, std::unordered_map<wchar_t, f2dGlyphInfo>& out, std::wstring& tex);
		private:
			fcyRefPointer<f2dTexture2D> m_pTex;
			std::unordered_map<wchar_t, f2dGlyphInfo> m_Charset;
			float m_fLineHeight;
		public:
			fFloat GetLineHeight();
			fFloat GetAscender();
			fFloat GetDescender();
			f2dTexture2D* GetCacheTexture();
			fResult CacheString(fcStrW String);
			fResult QueryGlyph(f2dGraphics* pGraph, fCharW Character, f2dGlyphInfo* InfoOut);
		public:
			HGEFont(std::unordered_map<wchar_t, f2dGlyphInfo>&& org, fcyRefPointer<f2dTexture2D> pTex);
		};
	private:
		fcyRefPointer<f2dFontProvider> m_pFontProvider;
		BlendMode m_BlendMode = BlendMode::MulAlpha;
		fcyColor m_BlendColor = fcyColor(0xFFFFFFFF);
	public:
		f2dFontProvider* GetFontProvider()LNOEXCEPT { return m_pFontProvider; }
		BlendMode GetBlendMode()const LNOEXCEPT { return m_BlendMode; }
		void SetBlendMode(BlendMode m)LNOEXCEPT { m_BlendMode = m; }
		fcyColor GetBlendColor()const LNOEXCEPT { return m_BlendColor; }
		void SetBlendColor(fcyColor c)LNOEXCEPT { m_BlendColor = c; }
	public:
		ResFont(const char* name, fcyRefPointer<f2dFontProvider> pFont);
	};

	/// @brief 资源池
	class ResourcePool
	{
	private:
		ResourceMgr* m_pMgr;
		ResourcePoolType m_iType;

		Dictionary<fcyRefPointer<ResTexture>> m_TexturePool;
		Dictionary<fcyRefPointer<ResSprite>> m_SpritePool;
		Dictionary<fcyRefPointer<ResAnimation>> m_AnimationPool;
		Dictionary<fcyRefPointer<ResParticle>> m_ParticlePool;
		Dictionary<fcyRefPointer<ResFont>> m_SpriteFontPool;
		Dictionary<fcyRefPointer<ResFont>> m_TTFFontPool;
	private:
		const wchar_t* getResourcePoolTypeName()
		{
			switch (m_iType)
			{
			case ResourcePoolType::Global:
				return L"全局资源池";
			case ResourcePoolType::Stage:
				return L"关卡资源池";
			default:
				return nullptr;
			}
		}
	public:
		/// @brief 清空对象池
		void Clear()LNOEXCEPT
		{
			m_TexturePool.clear();
			m_SpritePool.clear();
			m_AnimationPool.clear();
			m_ParticlePool.clear();
			m_SpriteFontPool.clear();
			m_TTFFontPool.clear();
		}

		/// @brief 检查资源是否存在
		/// @warning 注意t可以是非法枚举量
		bool CheckResourceExists(ResourceType t, const std::string& name)const LNOEXCEPT
		{
			switch (t)
			{
			case ResourceType::Texture:
				return m_TexturePool.find(name.c_str()) != m_TexturePool.end();
			case ResourceType::Sprite:
				return m_SpritePool.find(name.c_str()) != m_SpritePool.end();
			case ResourceType::Animation:
				return m_AnimationPool.find(name.c_str()) != m_AnimationPool.end();
			case ResourceType::Music:
				break;
			case ResourceType::SoundEffect:
				break;
			case ResourceType::Particle:
				return m_ParticlePool.find(name.c_str()) != m_ParticlePool.end();
			case ResourceType::SpriteFont:
				return m_SpriteFontPool.find(name.c_str()) != m_SpriteFontPool.end();
			case ResourceType::TrueTypeFont:
				return m_TTFFontPool.find(name.c_str()) != m_TTFFontPool.end();
			default:
				break;
			}

			return false;
		}

		/// @brief 导出资源表
		/// @note 在L的堆栈上放置一个table用以存放ResourceType中的资源名称
		/// @warning 注意t可以是非法枚举量
		void ExportResourceList(lua_State* L, ResourceType t)const LNOEXCEPT;

		/// @brief 装载纹理
		/// @param name 名称
		/// @param path 路径
		/// @param mipmaps 纹理链
		bool LoadTexture(const char* name, const std::wstring& path, bool mipmaps = true)LNOEXCEPT;

		/// @brief 装载纹理（UTF-8）
		LNOINLINE bool LoadTexture(const char* name, const char* path, bool mipmaps = true)LNOEXCEPT;
		
		/// @brief 装载图像
		LNOINLINE bool LoadImage(const char* name, const char* texname,
			double x, double y, double w, double h, double a, double b, bool rect = false)LNOEXCEPT;

		LNOINLINE bool LoadAnimation(const char* name, const char* texname,
			double x, double y, double w, double h, int n, int m, int intv, double a, double b, bool rect = false)LNOEXCEPT;

		/// @brief 装载粒子
		bool LoadParticle(const char* name, const std::wstring& path, const char* img_name, double a, double b, bool rect = false)LNOEXCEPT;

		LNOINLINE bool LoadParticle(const char* name, const char* path, const char* img_name, double a, double b, bool rect = false)LNOEXCEPT;

		/// @brief 装载纹理字体(HGE)
		bool LoadSpriteFont(const char* name, const std::wstring& path, bool mipmaps = true)LNOEXCEPT;

		/// @brief 装载纹理字体(fancy2d)
		bool LoadSpriteFont(const char* name, const std::wstring& path, const std::wstring& tex_path, bool mipmaps = true)LNOEXCEPT;

		LNOINLINE bool LoadSpriteFont(const char* name, const char* path, bool mipmaps = true)LNOEXCEPT;

		LNOINLINE bool LoadSpriteFont(const char* name, const char* path, const char* tex_path, bool mipmaps = true)LNOEXCEPT;

		/// @brief 装载TTF字体
		bool LoadTTFFont(const char* name, const std::wstring& path, float width, float height)LNOEXCEPT;

		LNOINLINE bool LoadTTFFont(const char* name, const char* path, float width, float height)LNOEXCEPT;

		/// @brief 获取纹理
		fcyRefPointer<ResTexture> GetTexture(const char* name)LNOEXCEPT
		{
			auto i = m_TexturePool.find(name);
			if (i == m_TexturePool.end())
				return nullptr;
			else
				return i->second;
		}

		/// @brief 获取精灵
		fcyRefPointer<ResSprite> GetSprite(const char* name)LNOEXCEPT
		{
			auto i = m_SpritePool.find(name);
			if (i == m_SpritePool.end())
				return nullptr;
			else
				return i->second;
		}

		/// @brief 获取动画
		fcyRefPointer<ResAnimation> GetAnimation(const char* name)LNOEXCEPT
		{
			auto i = m_AnimationPool.find(name);
			if (i == m_AnimationPool.end())
				return nullptr;
			else
				return i->second;
		}
		
		/// @brief 获取粒子系统
		fcyRefPointer<ResParticle> GetParticle(const char* name)LNOEXCEPT
		{
			auto i = m_ParticlePool.find(name);
			if (i == m_ParticlePool.end())
				return nullptr;
			else
				return i->second;
		}

		/// @brief 获取纹理字体
		fcyRefPointer<ResFont> GetSpriteFont(const char* name)LNOEXCEPT
		{
			auto i = m_SpriteFontPool.find(name);
			if (i == m_SpriteFontPool.end())
				return nullptr;
			else
				return i->second;
		}

		/// @brief 获取TTF字体
		fcyRefPointer<ResFont> GetTTFFont(const char* name)LNOEXCEPT
		{
			auto i = m_TTFFontPool.find(name);
			if (i == m_TTFFontPool.end())
				return nullptr;
			else
				return i->second;
		}
	private:
		ResourcePool& operator=(const ResourcePool&);
		ResourcePool(const ResourcePool&);
	public:
		ResourcePool(ResourceMgr* mgr, ResourcePoolType t)
			: m_pMgr(mgr), m_iType(t) {}
	};

	/// @brief 资源包
	class ResourcePack
	{
	private:
		std::wstring m_Path;
		std::wstring m_PathLowerCase;
		std::string m_Password;

		unzFile m_zipFile;
	public:
		/// @brief 获得资源包的实际路径
		const std::wstring& GetPath()const LNOEXCEPT { return m_Path; }
		/// @brief 获得资源包的实际路径小写名称
		const std::wstring& GetPathLowerCase()const LNOEXCEPT { return m_PathLowerCase; }
		/// @brief 尝试在资源包中定位并加载文件到内存
		/// @param[in] path 相对路径
		/// @param[out] outBuf 导出的文件数据
		/// @return 失败返回false，成功返回true
		bool LoadFile(const wchar_t* path, std::vector<char>& outBuf)LNOEXCEPT;
	protected:
		ResourcePack& operator=(const ResourcePack&);
		ResourcePack(const ResourcePack&);
	public:
		/// @brief 尝试在指定路径加载资源包
		/// @exception 失败抛出异常
		ResourcePack(const wchar_t* path, const char* passwd);
		~ResourcePack();
	};

	/// @brief 资源管理器
	class ResourceMgr
	{
	private:
		std::list<ResourcePack> m_ResPackList;

		float m_GlobalImageScaleFactor = 1.0f;
		ResourcePoolType m_ActivedPool = ResourcePoolType::Global;
		ResourcePool m_GlobalResourcePool;
		ResourcePool m_StageResourcePool;
	public:
		float GetGlobalImageScaleFactor()const LNOEXCEPT{ return m_GlobalImageScaleFactor; }
		void SetGlobalImageScaleFactor(float s)LNOEXCEPT{ m_GlobalImageScaleFactor = s; }

		/// @brief 获得当前激活的资源池类型
		ResourcePoolType GetActivedPoolType()LNOEXCEPT
		{
			return m_ActivedPool;
		}

		/// @brief 设置当前激活的资源池类型
		void SetActivedPoolType(ResourcePoolType t)LNOEXCEPT
		{
			m_ActivedPool = t;
		}

		/// @brief 获得当前激活的资源池
		ResourcePool* GetActivedPool()LNOEXCEPT
		{
			return GetResourcePool(m_ActivedPool);
		}

		/// @brief 获得资源池
		ResourcePool* GetResourcePool(ResourcePoolType t)LNOEXCEPT
		{
			switch (t)
			{
			case ResourcePoolType::Global:
				return &m_GlobalResourcePool;
			case ResourcePoolType::Stage:
				return &m_StageResourcePool;
			default:
				return nullptr;
			}
		}

		/// @brief 加载资源包
		/// @param[in] path 路径
		/// @param[in] passwd 密码
		bool LoadPack(const wchar_t* path, const char* passwd)LNOEXCEPT;

		/// @brief 卸载资源包
		/// @param[in] path 路径
		void UnloadPack(const wchar_t* path)LNOEXCEPT;

		/// @brief 卸载所有资源包
		void UnloadAllPack()LNOEXCEPT { m_ResPackList.clear(); }

		/// @brief 卸载所有资源并重置状态
		void ClearAllResource()LNOEXCEPT
		{
			m_GlobalResourcePool.Clear();
			m_StageResourcePool.Clear();
			m_ActivedPool = ResourcePoolType::Global;
			m_GlobalImageScaleFactor = 1.;
		}

		/// @brief 加载资源包（UTF8）
		/// @param[in] path 路径
		/// @param[in] passwd 密码
		LNOINLINE bool LoadPack(const char* path, const char* passwd)LNOEXCEPT;

		/// @brief 卸载资源包（UTF8）
		/// @param[in] path 路径
		LNOINLINE void UnloadPack(const char* path)LNOEXCEPT;

		/// @brief 装载文件
		/// @param[in] path 路径
		/// @param[out] outBuf 输出缓冲
		LNOINLINE bool LoadFile(const wchar_t* path, std::vector<char>& outBuf)LNOEXCEPT;

		/// @brief 装载文件（UTF8）
		/// @param[in] path 路径
		/// @param[out] outBuf 输出缓冲
		LNOINLINE bool LoadFile(const char* path, std::vector<char>& outBuf)LNOEXCEPT;

		/// @brief 解压资源文件
		/// @param[in] path 路径
		/// @param[in] target 目的地
		bool ExtractRes(const wchar_t* path, const wchar_t* target)LNOEXCEPT;

		/// @brief 解压资源文件（UTF8）
		/// @param[in] path 路径
		/// @param[in] target 目的地
		LNOINLINE bool ExtractRes(const char* path, const char* target)LNOEXCEPT;

		/// @brief 寻找纹理
		fcyRefPointer<ResTexture> FindTexture(const char* texname)LNOEXCEPT
		{
			fcyRefPointer<ResTexture> tRet;
			if (!(tRet = m_StageResourcePool.GetTexture(texname)))
				tRet = m_GlobalResourcePool.GetTexture(texname);
			return tRet;
		}

		/// @brief 获取纹理大小
		bool GetTextureSize(const char* texname, fcyVec2& out)LNOEXCEPT
		{
			fcyRefPointer<ResTexture> tRet = FindTexture(texname);
			if (!tRet)
				return false;
			out.x = static_cast<float>(tRet->GetTexture()->GetWidth());
			out.y = static_cast<float>(tRet->GetTexture()->GetHeight());
			return true;
		}

		/// @brief 寻找精灵
		fcyRefPointer<ResSprite> FindSprite(const char* name)LNOEXCEPT
		{
			fcyRefPointer<ResSprite> tRet;
			if (!(tRet = m_StageResourcePool.GetSprite(name)))
				tRet = m_GlobalResourcePool.GetSprite(name);
			return tRet;
		}

		/// @brief 寻找动画
		fcyRefPointer<ResAnimation> FindAnimation(const char* name)LNOEXCEPT
		{
			fcyRefPointer<ResAnimation> tRet;
			if (!(tRet = m_StageResourcePool.GetAnimation(name)))
				tRet = m_GlobalResourcePool.GetAnimation(name);
			return tRet;
		}

		/// @brief 寻找粒子
		fcyRefPointer<ResParticle> FindParticle(const char* name)LNOEXCEPT
		{
			fcyRefPointer<ResParticle> tRet;
			if (!(tRet = m_StageResourcePool.GetParticle(name)))
				tRet = m_GlobalResourcePool.GetParticle(name);
			return tRet;
		}

		/// @brief 寻找字体
		fcyRefPointer<ResFont> FindSpriteFont(const char* name)LNOEXCEPT
		{
			fcyRefPointer<ResFont> tRet;
			if (!(tRet = m_StageResourcePool.GetSpriteFont(name)))
				tRet = m_GlobalResourcePool.GetSpriteFont(name);
			return tRet;
		}

		/// @brief 寻找字体
		fcyRefPointer<ResFont> FindTTFFont(const char* name)LNOEXCEPT
		{
			fcyRefPointer<ResFont> tRet;
			if (!(tRet = m_StageResourcePool.GetTTFFont(name)))
				tRet = m_GlobalResourcePool.GetTTFFont(name);
			return tRet;
		}
	public:
		ResourceMgr();
	};
}
