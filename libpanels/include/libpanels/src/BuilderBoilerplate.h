/* DO NOT INCLUDE THIS FILE DIRECTLY */

#define LIBPANELS_INTERNAL_BUILD_PARAMETERS(name) \
	private: \
		BuildSolver<name>& internal_ret_solver; \
		name& solver; \
	public: \
		BuildSolver<name>& finish_parameters() \
		{                                               \
			BuildSolver<name>& to_ret = internal_ret_solver; \
            delete this;                                      \
			return to_ret; \
		} \
		BuildParameters(BuildSolver<name>& ret, name& solv) : internal_ret_solver(ret), solver(solv) \
		{} \
	private:

#define LIBPANELS_INTERNAL_BUILD_GEOMETRY(name) \
	private: \
		Geometry<S> geom; \
		BuildSolver<name>& internal_ret_solver; \
		name& solver; \
	public: \
		BuildSolver<name>& finish_geometry() \
		{ \
			BuildSolver<name>& to_ret = internal_ret_solver; \
            delete this;                                      \
			return to_ret; \
		} \
		BuildGeometry(Geometry<S> g, BuildSolver<name>& ret, name& solv) : geom(g), internal_ret_solver(ret), solver(solv) \
		{} \
	private:
