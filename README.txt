��ʼ��������
����1: �Ƿ������Է���PSI
����2: FPS: ÿ��֡��
MP2TMuxer *mMuxer = new MP2TMuxer(false, 25.);

������Ŀ
Program *mProgram = new Program();

����A/V��
Program::Stream *mStreamVideo = new Program::StreamVideo(Program::Stream::STREAMSUBTYPE_H264_VIDEO, 100);
Program::Stream *mStreamAudio = new Program::StreamAudio(Program::Stream::STREAMSUBTYPE_ISO_IEC_13818_3_AUDIO, 101);

������ӵ���Ŀ��	
mProgram->AddStream(mStreamVideo);
mProgram->AddStream(mStreamAudio);

���ý�Ŀ��PCR
mProgram->PCRPID(mStreamVideo->ElementaryPID());

����Ŀ��ӵ���������	
mMuxer->AddProgram(mProgram);

���ø����������ݷַ�����
������@realPacketsDeliverer �д����ú������
mMuxer->SetPacketsDeliverer(realPacketsDeliverer, this);

//*PES�ڲ�������Ҫ�޸ĵĵط�,PTS�ļ��㻹��Ҫ�޸�
PES pes(<NALU���ݳ���>);
pes.Fill(<NALU���ݳ���>, ESDT_VIDEO, stream->udwFrameType == 1);
//pes.SetPTS((stream->dwTimeTag * 1000000 + stream->dwMillisTag) * 0.09 /*90000.0 / 1000000.*/);
mMuxer->Mux(mStreamVideo, pes.Data(), pes.Size());
//mMuxer->Mux(mStreamAudio, pes.Data(), pes.Size());
mMuxer->FrameCount()++;