#ifndef MIPPPIPE_H
#define MIPPPIPE_H
#include "../utils/utils.h"
#include "mppframe.h"
#include "hi_math.h"
#include "hi_buffer.h"
#include "hi_common.h"
#include "hi_comm_vb.h"
#include "hi_comm_sys.h"
#include "hi_comm_vpss.h"
#include "hi_comm_video.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_sys.h"
#include "mpi_vpss.h"
#include <iostream>
#include <functional>
#include <iostream>
#include <time.h>
#include "sample_comm.h"

class MppPipe {
public:
	    static bool init(int Pipe, int Chn);
		static bool info(int pipe, int chn);
		static bool getFrame(VIFrame &outFrame, int pipe, int chn, int timeout);
		static bool getFrame(VIFrame &outFrame_bgr,VIFrame &outFrame_ir, int pipe_brg,int pipe_ir, int chn, int timeout);
private:
		static bool dumpFrame(VIDEO_FRAME_INFO_S &frame);
		static bool convertFrame(VIDEO_FRAME_S *frame, VIFrame *outFrame);

		static int sMaxPhyChn;
};

#endif

