// NautilusShell.cpp

// Spencer T. Parkin

#include <GL/glut.h>
#include <HappyMath.h>
#include <SpaceCurve.h>
#include <Tube.h>
#include <math.h>

// BUG: There's a seem you can see between
//      the chamber partitions and the chambers.
//			--FIXED by recomputing light norms.
// BUG: There is round-off error in the z-buffer
//      calculations that causes ugly jaggidy
//      crusties near the seems of the chamber
//      wall partitions.
// BUG: The curvature of the chamber partitions
//      isn't exactly right.
// BUG: The 2D chamber curves weren't used in
//      modeling the 3D chamber curves.
// BUG: When flat shaded, the inner part of the
//      spiral curve is dark.  This might be due
//      to round-off error in calculating the
//      surface normals of very small triangles.

enum
{
	CAM_MODE_VIEW_MODEL,
	CAM_MODE_FLY_INSIDE,
}
cam_mode = CAM_MODE_VIEW_MODEL;
float cam_t = 0.f;

int do_color = 0;
int do_clip_plane = 1;
int do_3d_model = 1;
int do_normals = 0;
int do_flat_shading = 0;

int mouse_x, mouse_y;

void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

GLfloat aspect_ratio = 1.f;
GLfloat near_plane = 0.01f;
GLfloat far_plane = 1000.f;
_vec rot = {0.f, 0.f, 0.f};
_vec trans = {0.f, 0.f, 0.f};
_vec *mouse_vec = 0;
GLfloat zoom = -3.f;

// These constants are used in defining the
// logarithmic curve of the nautilus shell.
// r = Ae^(Bt)
const float A = 1.f;
const float B = 0.2f;
const float C = PI / 5.f;		// This is the chamger size in radians.

// These functions define the logarithmic
// curve of the nautilus shell.
void naut_shell_curve0(_vec *dst, float t);
void naut_shell_curve1(_vec *dst, float t);
void naut_shell_curve2(_vec *dst, float t);
_space_curve naut_shell_curve =
{
	naut_shell_curve0,
	naut_shell_curve1,
	naut_shell_curve2,
};
int naut_shell_revolutions = 5;
float naut_shell_radius(float t);
void make_naut_shell(_tri_list *tl);
void make_naut_shell_chamber_partition(_tri_list *tl, int num_layers, float t);
void make_naut_shell_chambers(_tri_list *tl);
const float dome_top_x = 0.45f;
const float dome_top_y = 0.05f;
const float dome_top = 0.7f;

_tri_list naut_shell_tl;
int naut_shell_sides = 12;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("A Nautilus Shell by Spencer T. Parkin");

	glClearColor(1.f, 1.f, 1.f, 0.f);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
//	glEnable(GL_CULL_FACE);
//	glFrontFace(GL_CCW);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {1.f, 1.f, 2.f, 0.f};
	GLfloat light_dir[] = {-1.f, -1.f, -2.f, 1.f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_dir);

	tri_list_zero(&naut_shell_tl);
	make_naut_shell(&naut_shell_tl);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	return 0;
}

void make_naut_shell(_tri_list *tl)
{
	make_tube_curve(tl,
					naut_shell_sides, 0.f, -GLfloat(naut_shell_revolutions) * TWO_PI, -C / 4.f,
					&naut_shell_curve, 0, 0, naut_shell_radius);	
	make_naut_shell_chambers(tl);
}

void naut_shell_curve0(_vec *dst, float t)
{
	float r = A * exp(B * t);
	vec_set(dst, r * cos(t), r * sin(t), 0.f);
}

void naut_shell_curve1(_vec *dst, float t)
{
	float r = A * exp(B * t);
	float st = sin(t);
	float ct = cos(t);
	vec_set(dst, -st + B * ct, ct + B * st, 0.f);
	vec_scale(dst, dst, r);
}

void naut_shell_curve2(_vec *dst, float t)
{
	float r = A * exp(B * t);
	float st = sin(t);
	float ct = cos(t);
	vec_set(dst, -2.f * B * st + (B*B - 1.f) * ct,
				2.f * B * ct + (B*B - 1.f) * st, 0.f);
	vec_scale(dst, dst, r);
}

float naut_shell_radius(float t)
{
	// This formula is a bit difficult to derive.
	// It equires comming up with a sequence
	// and finding a convergent geometric series.
	// I did this on paper.
	float r = A * exp(B * t);
	float s = exp(B * TWO_PI);
	return r * (s - 1.f) / (s + 1.f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.f, aspect_ratio, near_plane, far_plane);	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if(cam_mode == CAM_MODE_VIEW_MODEL)
	{
		glTranslatef(trans.x, trans.y, zoom);
		glRotatef(DEGS(rot.x), 1.f, 0.f, 0.f);
		glRotatef(DEGS(rot.y), 0.f, 1.f, 0.f);
//		glRotatef(DEGS(rot.z), 0.f, 0.f, 1.f);
	}
	else
	{
		_vec cam_pos, cam_look_at;
		SC_calc_deriv(&naut_shell_curve, &cam_pos, cam_t);
		SC_calc_deriv(&naut_shell_curve, &cam_look_at, cam_t, 1);
		vec_scale(&cam_look_at, &cam_look_at, -1.f);
		vec_rot_z_axis(&cam_look_at, &cam_look_at, -PI / 6.f + rot.z);
		vec_add(&cam_look_at, &cam_pos, &cam_look_at);
		gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z,
				cam_look_at.x, cam_look_at.y, cam_look_at.z,
				0.f, 0.f, 1.f);
	}
	
	GLdouble plane_equ[] = {0.f, 0.f, -1.f, 0.f};
	glClipPlane(GL_CLIP_PLANE0, plane_equ);
	if(do_clip_plane && do_3d_model)
		glEnable(GL_CLIP_PLANE0);
	else
		glDisable(GL_CLIP_PLANE0);

	if(do_3d_model)
	{
		glEnable(GL_LIGHTING);
		GLfloat amb_dif[4] = {0.8f, 0.8f, 0.8f, 0.f};
		glBegin(GL_TRIANGLES);	
		_tri *tri;
		float t = 0.f;
		float dt = 1.f / GLfloat(naut_shell_tl.tri_count/* - 1*/);	// There will be more than one tri.
//		dt *= 3.5f;	// hack
		if(!do_color)	
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_dif);		
		for(tri = naut_shell_tl.head; tri; tri = tri->next)
		{			
			if(do_color)
			{
				amb_dif[0] = 0.5f * t;
				amb_dif[1] = 2.f * t * (1.f - t);
				amb_dif[2] = 1.f - t;
				amb_dif[3] = 0.f;				
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, amb_dif);
			}
			glColor3f(t * 0.5f, t * 0.5f, t * 0.5f);
			t += dt;
			if(do_flat_shading)
				glNormal3fv(&tri->norm.x);
			if(!do_flat_shading)
				glNormal3fv(&tri->light_norm[0].x);
			glVertex3fv(&tri->vtx[0].x);
			if(!do_flat_shading)
				glNormal3fv(&tri->light_norm[1].x);
			glVertex3fv(&tri->vtx[1].x);
			if(!do_flat_shading)
				glNormal3fv(&tri->light_norm[2].x);
			glVertex3fv(&tri->vtx[2].x);
		}
		glEnd();
		glDisable(GL_LIGHTING);
		if(do_normals)
		{
			_vec u, v;
			int i;
			glBegin(GL_LINES);			
			for(tri = naut_shell_tl.head; tri; tri = tri->next)
			{
				// Draw the surface normals.
				glColor3f(1.f, 0.f, 0.f);
				vec_add(&v, &tri->vtx[0], &tri->vtx[1]);
				vec_add(&v, &v, &tri->vtx[2]);
				vec_scale(&v, &v, 1.f / 3.f);
				glVertex3fv(&v.x);
				vec_scale(&u, &tri->norm, 0.1f);
				vec_add(&v, &v, &u);
				glVertex3fv(&v.x);
				// Draw the vertex lighting normals.
				glColor3f(0.f, 1.f, 0.f);
				for(i = 0; i < 3; i++)
				{
					glVertex3fv(&tri->vtx[i].x);
					vec_scale(&v, &tri->light_norm[i], 0.2f);
					vec_add(&v, &tri->vtx[i], &v);
					glVertex3fv(&v.x);
				}
			}
			glEnd();
		}
	}
	else
	{
		_vec v;
		GLfloat r;	
		glLineWidth(2.5f);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);
		glBegin(GL_LINE_STRIP);
		glColor3f(0.f, 0.f, 0.f);
		float t, t0 = 0.f, t1 = -GLfloat(naut_shell_revolutions) * TWO_PI;
		float dt = -0.05;	
		for(t = t0; fabs(t - t1) > fabs(2.f*dt); t += dt)
		{
			naut_shell_curve0(&v, t);
			glVertex3fv(&v.x);
		}
		glEnd();	
		glDisable(GL_LINE_STIPPLE);
		glBegin(GL_LINE_STRIP);
		glColor3f(0.f, 0.f, 0.f);
		for(t = t0; fabs(t - t1) > fabs(2.f*dt); t += dt)
		{
			r = A * exp(B * t) + naut_shell_radius(t);
			glVertex3f(r * cos(t), r * sin(t), 0.f);
		}
		glEnd();
		float r0, r1, dr, s, ds, q;
		dt = -C;
		for(t = t0; fabs(t - t1) > fabs(2.f*dt); t += dt)
		{
			glBegin(GL_LINE_STRIP);
			glColor3f(0.f, 0.f, 0.f);
			ds = 0.05f;
			for(s = 0.f; s <= 1.f + ds; s += ds)
			{
				q = 4.f * s * (1.f - s) * C;
				r = A * exp(B * (t - q));
				dr = naut_shell_radius(t - q);
				r0 = r - dr;
				r1 = r + dr;
				r = r0 + s*s * (r1 - r0);	// Not exactly a LERP.
				glVertex2f(r * cos(t - q), r * sin(t - q));				
			}
			glEnd();			
		}
	}

	glutSwapBuffers();
}

void make_naut_shell_chambers(_tri_list *tl)
{
	float t, t0 = 0.f, t1 = -GLfloat(naut_shell_revolutions) * TWO_PI;
	float dt = -C;
	for(t = t0; fabs(t - t1) > fabs(2.f*dt); t += dt)
		make_naut_shell_chamber_partition(tl, naut_shell_sides, t);
}

void make_naut_shell_chamber_partition(_tri_list *tl, int num_layers, float t)
{
	_vec trans;
	_vec tang, norm, binorm;
	SC_calc_deriv(&naut_shell_curve, &trans, t);
	SC_calc_TNB_frame(&naut_shell_curve, &tang, &norm, &binorm, t);
	vec_scale(&tang, &tang, -1.f);
	int r0 = 0, r1 = 1;
//	_vec ring[2][naut_shell_sides];
	_vec *ring[2];
	ring[0] = new _vec[naut_shell_sides];
	ring[1] = new _vec[naut_shell_sides];
	_tri *tri;	
	_vec top, base, *vptr;
	float ns_r = naut_shell_radius(t);
//	ns_r -= ns_r * 0.02f;
//	vec_set(&base, dome_top_x, dome_top_y, 0.f);
	vec_set(&base, 0.f, 0.f, 0.f);
	vec_scale(&base, &base, ns_r);
	vec_rot_sys(&base, &base, &norm, &binorm, &tang);
	vec_add(&base, &base, &trans);
	float r, rp, dr = 1.f / float(num_layers);
	float a, ap, da = TWO_PI / float(naut_shell_sides);
	float dx, dy, d;
	float x, y, z;
	int i, j, k;
	for(r = 0.f, i = 0; i <= num_layers; i++, r += dr)
	{	
		for(a = 0.f, j = 0; j < naut_shell_sides; j++, a += da)
		{
			x = cos(a);
			y = sin(a);
			dx = x - dome_top_x;
			dy = y - dome_top_y;
			d = sqrt(dx * dx + dy * dy);
			rp = r * d;
			ap = atan2(y - dome_top_y, x - dome_top_x);
			x = dome_top_x + rp * cos(ap);
			y = dome_top_y + rp * sin(ap);			
			z = dome_top * (1.f - r * r);
			if(r == 0.f)
			{
				vec_set(&top, x, y, z);
				vec_scale(&top, &top, ns_r);
				vec_rot_sys(&top, &top, &norm, &binorm, &tang);
				vec_add(&top, &top, &trans);
				break;
			}
			else
			{
//				vptr = &ring[j][r0];
				vptr = &ring[r0][j];
				vec_set(vptr, x, y, z);
				vec_scale(vptr, vptr, ns_r);
				vec_rot_sys(vptr, vptr, &norm, &binorm, &tang);
				vec_add(vptr, vptr, &trans);
			}
		}

		if(i == 0)
			continue;
		else if(i == 1) // Make the tri-fan at the top.
		{
			for(j = 0; j < naut_shell_sides; j++)
			{
				k = (j + 1) % naut_shell_sides;
				tri = new _tri;
//				tri_setup(tri, &top, &ring[j][r0], &ring[k][r0]);
				tri_setup(tri, &top, &ring[r0][j], &ring[r0][k]);
				tri_calc_light_norms(tri, &base, 1);
				vec_scale(&tri->light_norm[0], &tri->light_norm[0], -1.f);
				vec_scale(&tri->light_norm[1], &tri->light_norm[1], -1.f);
				vec_scale(&tri->light_norm[2], &tri->light_norm[2], -1.f);
				tri_list_append(tl, tri);
			}
		}
		else	// Make a quad-strip.
		{
			for(j = 0; j < naut_shell_sides; j++)
			{
				k = (j + 1) % naut_shell_sides;
				tri = new _tri;
//				tri_setup(tri, &ring[j][r0], &ring[k][r1], &ring[j][r1]);
				tri_setup(tri, &ring[r0][j], &ring[r1][k], &ring[r1][j]);
				tri_calc_light_norms(tri, &base, 1);
				vec_scale(&tri->light_norm[0], &tri->light_norm[0], -1.f);
				vec_scale(&tri->light_norm[1], &tri->light_norm[1], -1.f);
				vec_scale(&tri->light_norm[2], &tri->light_norm[2], -1.f);
				tri_list_append(tl, tri);
				tri = new _tri;
//				tri_setup(tri, &ring[j][r0], &ring[k][r0], &ring[k][r1]);
				tri_setup(tri, &ring[r0][j], &ring[r0][k], &ring[r1][k]);
				tri_calc_light_norms(tri, &base, 1);
				vec_scale(&tri->light_norm[0], &tri->light_norm[0], -1.f);
				vec_scale(&tri->light_norm[1], &tri->light_norm[1], -1.f);
				vec_scale(&tri->light_norm[2], &tri->light_norm[2], -1.f);
				tri_list_append(tl, tri);
			}
		}

		r0 ^= r1;
		r1 ^= r0;
		r0 ^= r1;
	}
	delete [] ring[0];
	delete [] ring[1];
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	aspect_ratio = GLfloat(w) / GLfloat(h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case '[':
		{
			if(cam_mode == CAM_MODE_VIEW_MODEL)
				zoom -= 0.05f;
			else if(cam_mode == CAM_MODE_FLY_INSIDE)
				cam_t -= 0.05f;
			break;
		}
		case ']':
		{
			if(cam_mode == CAM_MODE_VIEW_MODEL)
				zoom += 0.05f;
			else if(cam_mode == CAM_MODE_FLY_INSIDE)
				cam_t += 0.05f;
			break;
		}
		/* This feature is disabled because the chambers are in the way.
		case ' ':
		{
			if(cam_mode == CAM_MODE_VIEW_MODEL)
				cam_mode = CAM_MODE_FLY_INSIDE;
			else
				cam_mode = CAM_MODE_VIEW_MODEL;
			break;
		}
		*/
		case 'c':
		case 'C':
		{
			do_color = 1 - do_color;
			break;
		}
		case 'p':
		case 'P':
		{
			do_clip_plane = 1 - do_clip_plane;
			break;
		}
		case 'm':
		case 'M':
		{
			do_3d_model = 1 - do_3d_model;
			break;
		}
		case 'n':
		case 'N':
		{
			do_normals = 1 - do_normals;
			break;
		}
		case 'f':
		case 'F':
		{
			do_flat_shading = 1 - do_flat_shading;
			break;
		}
		case ',':
		case '<':
		{
			if(naut_shell_sides > 3)
			{
				tri_list_delete(&naut_shell_tl);
				naut_shell_sides--;
				make_naut_shell(&naut_shell_tl);
			}
			break;
		}
		case '.':
		case '>':
		{
			tri_list_delete(&naut_shell_tl);
			naut_shell_sides++;
			make_naut_shell(&naut_shell_tl);			
			break;
		}
		case 't':
		case 'T':
		{
			printf("number of triangles = %d\n", naut_shell_tl.tri_count);
			break;
		}
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		mouse_x = x;
		mouse_y = y;

		if(button == GLUT_LEFT_BUTTON)
			mouse_vec = &rot;
		else
			mouse_vec = &trans;
	}
}

void motion(int x, int y)
{
	int dx = x - mouse_x;
	int dy = y - mouse_y;
	mouse_x = x;
	mouse_y = y;
	if(mouse_vec == &rot)
	{
		if(cam_mode == CAM_MODE_VIEW_MODEL)
		{
			rot.x += GLfloat(dy) * 0.01f;
			rot.y += GLfloat(dx) * 0.01f;
		}
		else
		{
			rot.z -= GLfloat(dx) * 0.01f;
		}
	}
	else if(mouse_vec == &trans)
	{
		if(cam_mode == CAM_MODE_VIEW_MODEL)
		{
			trans.x += GLfloat(dx) * 0.01f;
			trans.y -= GLfloat(dy) * 0.01f;
		}
	}
	glutPostRedisplay();
}

// endof NautilusShell.cpp