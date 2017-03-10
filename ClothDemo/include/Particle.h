#ifndef PARTICLE_H
#define PARTICLE_H

class Particle
{
private:
	bool movable;

	float mass;
	float inv_mass;
	Vector3 pos;
	Vector3 old_pos;
	Vector3 acceleration;
	
public:
	Particle(Vector3 pos) : pos(pos), old_pos(pos),acceleration(Vector3(0,0,0)), mass(PARTICLE_MASS), inv_mass(1.0f/PARTICLE_MASS), movable(true){}
	Particle(){}

	void AddForce(Vector3 f)
	{
		acceleration += f * inv_mass;
	}

	// Verlet Integration
	void Update()
	{
		if(movable)
		{
			Vector3 temp = pos;
			pos = pos + (pos-old_pos)*(1.0f-DAMPING) + acceleration*TIME_STEPSIZE2;
			old_pos = temp;
			acceleration = Vector3(0,0,0); // Reset acceleration	
		}
	}

	Vector3& getPos() {return pos;}

	void OffsetPos(const Vector3 v) { if(movable) pos += v;}

	void MakeUnmovable() {movable = false;}

};

#endif