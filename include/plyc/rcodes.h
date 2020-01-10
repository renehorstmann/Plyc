#ifndef PLYC_RCODES_H
#define PLYC_RCODES_H
#ifdef __cplusplus
extern "C" {
#endif


#define SetErrGoto(err, set, label) { (err) = (set); goto label; }

typedef const char *ply_err;

/** ply_err for successful operation, is a NULL pointer */
#define PLY_Success (ply_err) 0


#ifdef __cplusplus
}
#endif
#endif //PLYC_RCODES_H
