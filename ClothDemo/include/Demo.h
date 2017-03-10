#ifndef DEMO_H
#define DEMO_H

//-----------------------------------------------------------------------------
// State ID Define
//-----------------------------------------------------------------------------
#define STATE_DEMO 0

//-----------------------------------------------------------------------------
// Menu Class
//-----------------------------------------------------------------------------
class Demo : public State
{
public:
	Demo();

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer( ViewerSetup *viewer );
	virtual void Update( float elapsed );
	virtual void Render();

private:
	SceneObject *m_viewer; // The viewer scene object is used as a camera.
	Cloth *m_cloth;
	float gustThreshold;

	char m_fpsText[16]; // Frame rate character string.
	Font *m_fpsFont; // Font for rendering the frame rate.
};

#endif