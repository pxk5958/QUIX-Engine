#ifndef _SPRITE_H
#define _SPRITE_H

	enum CollisionType
	{
		COLLISION_NONE = 0,
		COLLISION_RECT = 1,
		COLLISION_DIST = 2
	};

	class Sprite
	{
		private:
			/*bool visible;
			bool alive;
			int lifetimeLength;
			Timer lifetimeTimer;
			int objecttype;*/
			Vector3 position;
			Vector3 velocity;
			bool imageLoaded;
			int state;
			int direction;
	
		protected:
			//Material *image;
			IDirect3DTexture9* image;
			//LPDIRECT3DTEXTURE9 texture;
			D3DXIMAGE_INFO info;
			int width,height;
			int animcolumns;
			int framestart,frametimer;
			int movestart, movetimer;
			bool collidable;
			enum CollisionType collisionMethod;
			int firstframe,lastframe,curframe,totalframes,animdir;
			float faceAngle, moveAngle;
			int animstartx, animstarty;
			float rotation, scaling;
			D3DXMATRIX matRotate;
			D3DXMATRIX matScale;
			void Transform();
			D3DCOLOR color;
			bool visible;
			bool alive;

		public:
			//screen position
			Vector3 GetPosition() { return position; }
			void SetPosition(Vector3 pos) { position = pos; }
			void SetPosition(float x, float y) { position.Set(x,y,0); }
			float GetX() { return position.getX(); }
			float GetY() { return position.getY(); }
			void SetX(float x) { position.setX(x); }
			void SetY(float y) { position.setY(y); }
			
			//movement velocity
			Vector3 GetVelocity() { return velocity; }
			void SetVelocity(Vector3 v) { velocity = v; }
			void SetVelocity(float x, float y) { velocity.setX(x); velocity.setY(y); }

			//image size
			void SetSize(int w, int h) { width = w; height = h; }
			int GetWidth() { return width; }
			void SetWidth(int value) { width = value; }
			int GetHeight() { return height; }
			void SetHeight(int value) { height = value; }

			bool GetVisible() { return visible; }
			void SetVisible(bool value) { visible = value; }
			bool GetAlive() { return alive; }
			void SetAlive(bool value) { alive = value; }

			int GetState() { return state; }
			void SetState(int value) { state = value; }
			int GetDirection() { return direction; }
			void SetDirection(int value) { direction = value; }
			int GetColumns() { return animcolumns; }
			void SetColumns(int value) { animcolumns = value; }
			int GetFrameTimer() { return frametimer; }
			void SetFrameTimer(int value) { frametimer = value; }
			int GetCurrentFrame() { return curframe; }
			void SetCurrentFrame(int value) { curframe = value; }
			int GetTotalFrames() { return totalframes; }
			void SetTotalFrames(int value) { totalframes = value; }
			int GetAnimationDirection() { return animdir; }
			void SetAnimationDirection(int value) { animdir = value; }
			float GetRotation() { return rotation; }
			void SetRotation(float value) { rotation = value; }
			float GetScale() { return scaling; }
			void SetScale(float value) { scaling = value; }
			void SetColor(D3DCOLOR col) { color = col; }
			int GetMoveTimer() { return movetimer; }
			void SetMoveTimer(int value) { movetimer = value; }
			bool IsCollidable() { return collidable; }
			void SetCollidable(bool value) { collidable = value; }
			CollisionType GetCollisionMethod() { return collisionMethod; }
			void SetCollisionMethod(CollisionType type) { collisionMethod = type; }

		public:
			Sprite();
			virtual ~Sprite();
			void Release();
			bool LoadImage(char* name, char* path);
			//void SetImage(Material *);
			void Move();
			void Animate();
			void Draw(float zorder = 0.0f);
	}; //class

#endif