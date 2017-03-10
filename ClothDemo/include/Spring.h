#ifndef SPRING_H
#define SPRING_H

class Spring
{
public:
	Spring(float _stiffness, float _damping, Particle* _start, Particle* _end, float length)
	{
		stiffness = _stiffness;
		damping   = _damping;
		restLength = length;
		start = _start;
		end   = _end;
	}

	~Spring();

	void applyForce()
	{
		Vector3 dir = start->getPos() - end->getPos();
		D3DXVECTOR3 direction = D3DXVECTOR3(dir.getX(), dir.getY(), dir.getZ());

		if( direction != D3DXVECTOR3(0,0,0) )
		{
			float currentLength = D3DXVec3Length(&direction);
			D3DXVec3Normalize(&direction, &direction);

			D3DXVECTOR3 force = -stiffness * ((currentLength - restLength) * direction);
			force -= damping * D3DXVec3Dot( &(start->velocity - end->velocity), &direction) * direction;

			start->force += force;
			end->force   -= force;
		}
	}

	void clearForce();

private:
	float stiffness;
	float damping;
	float restLength;

	Particle* start;
	Particle* end;
};

#endif