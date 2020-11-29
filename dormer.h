void	initdm(int*, char*);
void	cfdorm(int*, char*);
void	cfcurs(int);
void	readev();

typedef
struct Dormer
{
	void	*dis;
	void	*win;
	void	*vis;

	struct{
		int etype;
		int	key;
		int	btn;
		int	x, y;
		int w, h;
	};
} Dormer;

extern Dormer dm;

enum{
	Ekbd, Ebtn, Eresize,
	Eptr, Equit
};
