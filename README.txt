初始化复用器
参数1: 是否周期性发送PSI
参数2: FPS: 每秒帧数
MP2TMuxer *mMuxer = new MP2TMuxer(false, 25.);

创建节目
Program *mProgram = new Program();

创建A/V流
Program::Stream *mStreamVideo = new Program::StreamVideo(Program::Stream::STREAMSUBTYPE_H264_VIDEO, 100);
Program::Stream *mStreamAudio = new Program::StreamAudio(Program::Stream::STREAMSUBTYPE_ISO_IEC_13818_3_AUDIO, 101);

将流添加到节目中	
mProgram->AddStream(mStreamVideo);
mProgram->AddStream(mStreamAudio);

设置节目的PCR
mProgram->PCRPID(mStreamVideo->ElementaryPID());

将节目添加到复用器中	
mMuxer->AddProgram(mProgram);

设置复用器的数据分发方法
可以在@realPacketsDeliverer 中处理复用后的数据
mMuxer->SetPacketsDeliverer(realPacketsDeliverer, this);

//*PES内部还有需要修改的地方,PTS的计算还需要修改
PES pes(<NALU数据长度>);
pes.Fill(<NALU数据长度>, ESDT_VIDEO, stream->udwFrameType == 1);
//pes.SetPTS((stream->dwTimeTag * 1000000 + stream->dwMillisTag) * 0.09 /*90000.0 / 1000000.*/);
mMuxer->Mux(mStreamVideo, pes.Data(), pes.Size());
//mMuxer->Mux(mStreamAudio, pes.Data(), pes.Size());
mMuxer->FrameCount()++;