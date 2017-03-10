#ifndef CLOTH_H
#define CLOTH_H

struct ClothVertex
{
	D3DXVECTOR3 translation;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texCoords;
};

#define CLOTH_VERTEX_FVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define CLOTH_VERTEX_FVF_SIZE D3DXGetFVFVertexSize( CLOTH_VERTEX_FVF )



class Cloth
{
private:
	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9  indexBuffer;
	LPD3DXEFFECT effect;
	IDirect3DTexture9* texture;
	D3DXHANDLE viewProjectionHandle;

	ClothVertex* vertices;
	WORD* indices;

	unsigned int tricount;
	unsigned int spacing;
	unsigned int width;
	unsigned int height;

	std::vector<Particle> particles;
	std::vector<Constraint> constraints;

	Particle* GetParticle(int x, int y) {return &particles[y*width + x];}

	void MakeConstraint(Particle *p1, Particle *p2) {constraints.push_back(Constraint(p1,p2));}


	void CreateConstraints()
	{
		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid)
		for(unsigned int x=0; x<width; x++)
		{
			for(unsigned int y=0; y<height; y++)
			{
				if (x<width-1) MakeConstraint(GetParticle(x,y),GetParticle(x+1,y));
				if (y<height-1) MakeConstraint(GetParticle(x,y),GetParticle(x,y+1));
				if (x<width-1 && y<height-1) MakeConstraint(GetParticle(x,y),GetParticle(x+1,y+1));
				if (x<width-1 && y<height-1) MakeConstraint(GetParticle(x+1,y),GetParticle(x,y+1));
			}
		}


		// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid)
		for(unsigned int x=0; x<width; x++)
		{
			for(unsigned int y=0; y<height; y++)
			{
				if (x<width-2) MakeConstraint(GetParticle(x,y),GetParticle(x+2,y));
				if (y<height-2) MakeConstraint(GetParticle(x,y),GetParticle(x,y+2));
				if (x<width-2 && y<height-2) MakeConstraint(GetParticle(x,y),GetParticle(x+2,y+2));
				if (x<width-2 && y<height-2) MakeConstraint(GetParticle(x+2,y),GetParticle(x,y+2));
			}
		}
	}

	void CreateVertexBuffer()
	{
		int vertexBufferSize = width * height * CLOTH_VERTEX_FVF_SIZE;

		void* vertexPointer;
		if( !FAILED( vertexBuffer->Lock( 0, vertexBufferSize, (void**)&vertexPointer, D3DLOCK_DISCARD ) ) )
		{
			memcpy(vertexPointer, vertices, vertexBufferSize);
			vertexBuffer->Unlock();
		}
	}

	void CreateIndexBuffer()
	{
		int indexBufferCount = (width - 1) * (height - 1) * 6;
		int indexBufferSize = indexBufferCount * sizeof(WORD);

		indices = new WORD[indexBufferCount];

		int i = 0; 
		for( unsigned int x = 0; x < width - 1; x++ )
		{
			for( unsigned int y = 0; y < height - 1; y++ )
			{
				// Lower Triangle
				indices[i++] =  x    + (y+1) * width;
				indices[i++] = (x+1) +  y    * width;
				indices[i++] =  x    +  y    * width;

				// Upper Triangle
				indices[i++] =  x    + (y+1) * width;
				indices[i++] = (x+1) + (y+1) * width;
				indices[i++] = (x+1) +  y    * width;
			}
		}

		g_engine->GetRenderer()->GetDevice()->CreateIndexBuffer( indexBufferSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0 );

		void* indexPointer;
		if( !FAILED( indexBuffer->Lock(0, indexBufferSize, (void**)&indexPointer, 0 )))
		{
			memcpy(indexPointer, indices, indexBufferSize);
			indexBuffer->Unlock();
		}
	}

	void CreateNormals()
	{
		#pragma omp parallel for
		for( unsigned int i = 0; i < tricount; i++ )
		{
			WORD index1 = indices[i * 3    ];
			WORD index2 = indices[i * 3 + 1];
			WORD index3 = indices[i * 3 + 2];

			D3DXVECTOR3 normal;
			D3DXVECTOR3 side1 = vertices[index1].translation - vertices[index2].translation;
			D3DXVECTOR3 side2 = vertices[index1].translation - vertices[index3].translation;
			D3DXVec3Cross(&normal, &side1, &side2);
			D3DXVec3Normalize(&normal, &normal);

			vertices[index1].normal = normal;
			vertices[index2].normal = normal;
			vertices[index3].normal = normal;
		}
	}
	
public:

	Cloth(float posX, float posY, float posZ, unsigned int width, unsigned int height, unsigned int spacing) : width(width), height(height), spacing(spacing), tricount((width-1)*(height-1)*2)
	{
		vertexBuffer = NULL;
		indexBuffer = NULL;

		vertices = new ClothVertex[ width*height ];

		particles.resize(width*height);

		#pragma omp parallel for
		for(unsigned int x=0; x < width; x++)
		{
			for(unsigned int y=0; y < height; y++)
			{
				Vector3 pos = Vector3(posX + (x * spacing) ,
								posY - (y * spacing),
								posZ);
				vertices[y*width+x].translation = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
				particles[y*width+x] = Particle(pos);

				vertices[y*width+x].texCoords.x = x / (float)(width - 1);
				vertices[y*width+x].texCoords.y = y / (float)(height - 1);
				
			}
		}

		/*
		// making the upper left most three and right most three particles unmovable
		for(int i=0;i<3; i++)
		{
			// upper left
			GetParticle(0+i , 0)->OffsetPos(Vector3(0.5f,0.0f,0.0f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			GetParticle(0+i , 0)->MakeUnmovable(); 

			// upper right
			GetParticle(width-1-i , 0)->OffsetPos(Vector3(-0.5f,0.0f,0.0f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			GetParticle(width-1-i , 0)->MakeUnmovable();


			// upper left
			//GetParticle(0 , 0+i)->OffsetPos(Vector3(0.0f,-6.0f,0.0f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//GetParticle(0 , 0+i)->MakeUnmovable(); 

			// lower left
			//GetParticle(0 , height-1-i)->OffsetPos(Vector3(0.0f,6.0f,0.0f)); // moving the particle a bit towards the center, to make it hang more natural - because I like it ;)
			//GetParticle(0 , height-1-i)->MakeUnmovable();
		}
		*/

		GetParticle(0 , 0)->OffsetPos(Vector3(0.0f,-6.0f,0.0f));
		GetParticle(0 , 0)->MakeUnmovable(); 

		GetParticle(0 , height-1)->OffsetPos(Vector3(0.0f,6.0f,0.0f));
		GetParticle(0 , height-1)->MakeUnmovable();

		CreateConstraints();

		
		// Load content

		std::string texfile("Assets\\Textures\\ManU.png");
		D3DXCreateTextureFromFileExA(g_engine->GetRenderer()->GetDevice(), texfile.c_str(), 0, 0, 0, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_FILTER_BOX, D3DX_FILTER_BOX, 0, 0, 0, &texture);
		

		g_engine->GetRenderer()->GetDevice()->SetFVF(CLOTH_VERTEX_FVF);
		g_engine->GetRenderer()->GetDevice()->CreateVertexBuffer( width*height*CLOTH_VERTEX_FVF_SIZE,
															D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, CLOTH_VERTEX_FVF, 
															D3DPOOL_DEFAULT, &vertexBuffer, NULL );
		
		CreateIndexBuffer();
		CreateNormals();
		CreateVertexBuffer();

	}

	~Cloth()
	{
		delete[] vertices;
		delete[] indices;

		if ( g_engine->GetRenderer()->GetDevice() ) 
		{
			g_engine->GetRenderer()->GetDevice()->SetStreamSource( 0, NULL, 0, 0 );
			g_engine->GetRenderer()->GetDevice()->SetIndices( NULL );
		}

		if ( vertexBuffer ) vertexBuffer->Release();
		if ( indexBuffer  ) indexBuffer->Release();
		vertexBuffer = NULL;
		indexBuffer  = NULL;

		// release particles and constraints if necessary

		texture->Release();
	}

	void Render()
	{
		g_engine->GetRenderer()->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		g_engine->GetRenderer()->GetDevice()->SetTexture(0, texture);
	
		g_engine->GetRenderer()->GetDevice()->SetFVF( CLOTH_VERTEX_FVF );   //???
		g_engine->GetRenderer()->GetDevice()->SetStreamSource(0, vertexBuffer, 0, CLOTH_VERTEX_FVF_SIZE);
		g_engine->GetRenderer()->GetDevice()->SetIndices(indexBuffer);
		g_engine->GetRenderer()->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, width*height, 0, tricount);

		
		g_engine->GetRenderer()->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}


	void Update()
	{
		#pragma omp parallel for
		for(unsigned int i=0; i<CONSTRAINT_ITERATIONS; i++) // iterate over all constraints several times
		{
			for( unsigned int j = 0; j < constraints.size(); j++ )
			{
				constraints[j].SatisfyConstraint(); // satisfy constraint.
			}
		}

		#pragma omp parallel for
		for( unsigned int i = 0; i < width*height; i++ )
		{
			particles[i].Update();
			vertices[i].translation = D3DXVECTOR3(particles[i].getPos().getX(), particles[i].getPos().getY(), particles[i].getPos().getZ());
		}

		//CreateNormals();
		CreateVertexBuffer();
	}

	// Used to add forces to 'particles'
	void AddForce(const Vector3 direction)
	{
		#pragma omp parallel for
		for( unsigned int i = 0; i < width*height; i++ )
		{
			particles[i].AddForce(direction); // add the forces to each particle
		}

	}

	// To save computation time in computing normals, adding wind forces to 'triangles'(which needs normals) has been combined with creating normals
	void AddWindForce_CreateNormals(const Vector3 direction)
	{
		#pragma omp parallel for
		for( unsigned int i = 0; i < tricount; i++ )
		{
			// Create normals
			WORD index1 = indices[i * 3    ];
			WORD index2 = indices[i * 3 + 1];
			WORD index3 = indices[i * 3 + 2];

			D3DXVECTOR3 normal;
			D3DXVECTOR3 side1 = vertices[index1].translation - vertices[index2].translation;
			D3DXVECTOR3 side2 = vertices[index1].translation - vertices[index3].translation;
			D3DXVec3Cross(&normal, &side1, &side2);
			D3DXVec3Normalize(&normal, &normal);

			vertices[index1].normal = normal;
			vertices[index2].normal = normal;
			vertices[index3].normal = normal;

			// Add wind force in the direction of normal of the triangle
			if( direction != Vector3(0.0f,0.0f,0.0f) )
			{
				Vector3 v3Normal = Vector3(normal.x, normal.y, normal.z);
				Vector3 force = v3Normal*(v3Normal.DotProduct(direction));
				particles[index1].AddForce(force);
				particles[index2].AddForce(force);
				particles[index3].AddForce(force);
			}
		}
	}

	void BallCollision(const Vector3 center,const float radius )
	{
		#pragma omp parallel for
		for( unsigned int i = 0; i < width*height; i++ )
		{
			Vector3 v = particles[i].getPos()-center;
			float l = v.Length();
			if ( v.Length() < radius) // if the particle is inside the ball
			{
				particles[i].OffsetPos(v.Normal()*(radius-l)); // project the particle to the surface of the ball
			}
		}
	}

};

#endif