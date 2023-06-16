#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include <cmath>

class sphere : public hittable {
public:
	sphere() {}
	sphere(point3 cen, float r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
	virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
	point3 center;
	float radius;
	shared_ptr<material> mat_ptr;

private:
	static void get_sphere_uv(const point3& p, float& u, float& v) {
		// p: a point on the sphere of radius 1, centered at the origin
		// u: returned value [0, 1] of angle around y axis from x=-1
		// v: returned value [0,1] of angle from y=-1 to y=+1
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;
	}
};

bool sphere::hit(const ray& r, const float t_min, const float t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	// find the nearest square root that lies in the acceptable range
	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root) {
			return false;
		}
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 outward_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, outward_normal);
	get_sphere_uv(outward_normal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
	output_box = aabb(
		center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius));
	return true;
}

#endif