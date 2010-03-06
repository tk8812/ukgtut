#pragma once

class CFSMObject
{
private:
	irr::s32 m_nStatus;
	irr::s32 m_nStep;

	std::string m_strMessage;
	
public:
	CFSMObject(void)
	{
		m_nStatus = 0;
		m_nStep = 0;
		m_strMessage = "nomessage";
	}

	virtual ~CFSMObject(void)
	{
	}

	void SetStatus(irr::s32 status)
	{
		m_nStep = 0; //Ω∫≈«√ ±‚»≠
		m_nStatus = status;
	}
	void SetStep(irr::s32 step)
	{
		m_nStep = step;
	}

	irr::s32 GetStep()
	{
		return m_nStep;
	}

	irr::s32 GetStatus()
	{
		return m_nStatus;
	}

	void IncStep()
	{
		m_nStep++;
	}
	void DecStep()
	{
		m_nStep--;
	}

	const std::string &GetMessage()
	{
		return m_strMessage;
	}

	void SetMessage(std::string strMsg)
	{
		m_strMessage = strMsg;
	}

	virtual void OnAnimate(irr::f32 fTick) = 0;
	//virtual irr::core::vector3df getPosition() = 0;
	virtual irr::core::vector3df getPosition() = 0;
	virtual bool isDie() = 0;

};
