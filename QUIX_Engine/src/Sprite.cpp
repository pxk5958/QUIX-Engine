#include "QUIX.h"

	Sprite::Sprite()
	{
		image = NULL;
		imageLoaded = false;
		SetPosition(0.0f,0.0f);
		SetVelocity(0.0f,0.0f);
		state = 0;
		direction = 0;
		width = 1;
		height = 1;
		firstframe = 0;
		lastframe = 0;
		curframe = 0;
		totalframes = 1;
		animdir = 1;
		animcolumns = 1;
		framestart = 0;
		frametimer = 0;
		animcolumns = 1;
		animstartx = 0;
		animstarty = 0;
		faceAngle = 0.0f;
		moveAngle = 0.0f;
		rotation = 0.0f;
		scaling = 1.0f;
		color = 0xFFFFFFFF;
		movetimer = 16;
		movestart = 0;
		collidable = true;
		collisionMethod = COLLISION_RECT;
		visible = true;
		alive = true;
	}

	Sprite::~Sprite()
	{
		Release();
	}

	void Sprite::Release()
	{
		if (imageLoaded)
		{
		//	if (image)
		//		g_engine->GetMaterialManager()->Remove(&image);
			SAFE_RELEASE(image);
		}
	}

	bool Sprite::LoadImage(char* name, char* path)
	{
		//if image already exists, free it
		Release();

		char* file = new char[strlen(name) + strlen(path) + 1];
		ZeroMemory( file, sizeof( char ) * ( strlen(name) + strlen(path) + 1 ) );
		strcpy( file, path );
		strcat( file, name );
		D3DXGetImageInfoFromFileA(file, &info);
		D3DXCreateTextureFromFileExA(g_engine->GetRenderer()->GetDevice(), file, info.Width, info.Height, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, &info, 0, &image);
		SetSize(info.Width,info.Height);
		SAFE_DELETE_ARRAY( file );
		//image =	g_engine->GetMaterialManager()->Add(name, path);
		//SetSize(image->GetWidth(),image->GetHeight());

		imageLoaded = true;

		return true;
	}

	/*
	void Sprite::SetImage(Material *img)
	{
		//if image already exists, free it
		Release();

		image = img;
		SetWidth(image->GetWidth());
		SetHeight(image->GetHeight());
		imageLoaded = true;
	}
	*/

	void Sprite::Transform()
	{
		D3DXMATRIX mat;
		D3DXVECTOR2 scale(scaling, scaling);
		D3DXVECTOR2 center((float)(width*scaling)/2, (float)(height*scaling)/2);
		D3DXVECTOR2 trans(GetX(), GetY());
		D3DXMatrixTransformation2D(&mat,NULL,0,&scale,&center,rotation, &trans);
		g_engine->GetRenderer()->GetSpriteHandler()->SetTransform(&mat);
	}

	void Sprite::Draw(float zorder)
	{
		//calculate source frame location
		int fx = (curframe % animcolumns) * width;
		int fy = (curframe / animcolumns) * height;
		RECT srcRect = {fx,fy, fx+width, fy+height};

		//draw the sprite frame
		Transform();
		g_engine->GetRenderer()->GetSpriteHandler()->Draw(image,&srcRect,NULL,&D3DXVECTOR3(0,0,zorder),color);
		//g_engine->GetRenderer()->GetSpriteHandler()->Draw(image->GetTexture(),&srcRect,NULL,NULL,color);

		g_engine->GetRenderer()->GetSpriteHandler()->SetTransform(D3DXMatrixIdentity(&D3DXMATRIX()));
	}

	void Sprite::Move()
	{
		if (movetimer > 0)
		{
			if (timeGetTime() > (DWORD)(movestart + movetimer))
			{
				//reset move timer
				movestart = timeGetTime();

				//move sprite by velocity amount
				SetX(GetX() + velocity.getX());
				SetY(GetY() + velocity.getY());
			}
		}
		else
		{
			//no movement timer- -update at cpu clock speed
			SetX(GetX() + velocity.getX());
			SetY(GetY() + velocity.getY());
		}
	}

	void Sprite::Animate()
	{
		//update frame based on animdir
		if (frametimer > 0)
		{
			if (timeGetTime() > (DWORD)(framestart + frametimer))
			{
				//reset animation timer
				framestart = timeGetTime();
				curframe += animdir;
			}
		}
		else
		{
			//no animation timer- -update at cpu clock speed
			curframe += animdir;
		}
		
		if (totalframes > 1)
		{
            if (lastframe == 0) lastframe = totalframes-1;
        }

		//keep frame within bounds	
		if (curframe < firstframe) curframe = lastframe;
		if (curframe > lastframe) curframe = firstframe;
	}