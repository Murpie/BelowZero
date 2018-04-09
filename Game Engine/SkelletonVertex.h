class SkelletonVertex
{
public:
	SkelletonVertex();
	~SkelletonVertex();

	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float tx, ty, tz;
	float bx, by, bz;
	float weight1 = 0.0, weight2 = 0.0, weight3 = 0.0, weight4 = 0.0;
	float ID1 = 0.0, ID2 = 0.0, ID3 = 0.0, ID4 = 0.0;
	bool SkelletonVertex::operator==(const SkelletonVertex &other);

private:

};
