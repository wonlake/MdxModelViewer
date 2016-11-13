#include "LoadBlpImage.h"

boolean SourceFill(jpeg_decompress_struct* info)
{
	my_source_manager* sourceManager;

	sourceManager = reinterpret_cast<my_source_manager*>(info->src);

	sourceManager->buffer = sourceManager->sourceBuffer;
	sourceManager->man.next_input_byte = sourceManager->buffer;
	sourceManager->man.bytes_in_buffer = sourceManager->sourceBufferSize;

	return true;
}

void SourceSkip(jpeg_decompress_struct* info, long NrOfBytes)
{
	my_source_manager* sourceManager;

	sourceManager = reinterpret_cast<my_source_manager*>(info->src);

	if(NrOfBytes > 0)
	{
		while(NrOfBytes > static_cast<long>(sourceManager->man.bytes_in_buffer))
		{
			NrOfBytes -= static_cast<long>(sourceManager->man.bytes_in_buffer);
			SourceFill(info);
		}

		sourceManager->man.next_input_byte += NrOfBytes;
		sourceManager->man.bytes_in_buffer -= NrOfBytes;
	}
}

void SourceInit(jpeg_decompress_struct* info){}

void SourceTerminate(jpeg_decompress_struct* info){}

bool LoadBlpImage(IMAGEINFO *ImageInfo,void*filename,int realFileName)
{
	FILE *fp;
	int size=0;
	unsigned char *Buffer;
	BLP_HEADER header;
	if(realFileName)
	{
		fp=fopen((char*)filename,"rb");
		if(fp==NULL)
			return false;
		fseek(fp,0,SEEK_END);
		size=ftell(fp);
		fseek(fp,0,SEEK_SET);
		Buffer=new unsigned char[size];
		fread(Buffer,sizeof(char),size,fp);
	}
	else
		Buffer=(unsigned char*)filename;
	memcpy(&header,Buffer,sizeof(BLP_HEADER));
	if(header.MagicNumber!='1PLB')
	{
		if(realFileName)
		{
			if(Buffer!=NULL)
				delete []Buffer;
			Buffer=NULL;
			fclose(fp);
		}
		return false;
	}
	else
	{
		switch(header.Compression)
		{
		case 0:
			{
				DWORD JpegHeaderSize;
				char *tempBuffer2=NULL;

				int i;
				int Stride;
				int Offset;
				char Opaque;
				JSAMPARRAY pointer;
				jpeg_decompress_struct Info;

				memcpy(&JpegHeaderSize, &Buffer[sizeof(BLP_HEADER)], sizeof(DWORD));

				tempBuffer2=new char[header.Size[0] + JpegHeaderSize];

				memcpy(tempBuffer2, &Buffer[sizeof(BLP_HEADER) + sizeof(DWORD)], JpegHeaderSize);
				memcpy(&tempBuffer2[JpegHeaderSize],&Buffer[header.Offset[0]], header.Size[0]);

				jpeg_error_mgr ErrorManager;

				Info.err = jpeg_std_error(&ErrorManager);
				jpeg_create_decompress(&Info);
				my_source_manager* sourceManager;
				int bufferSize=header.Size[0] + JpegHeaderSize;

				Info.src = reinterpret_cast<jpeg_source_mgr*>((Info.mem->alloc_small)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_PERMANENT, sizeof(my_source_manager)));
				sourceManager = reinterpret_cast<my_source_manager*>(Info.src);

				sourceManager->buffer = reinterpret_cast<JOCTET*>((Info.mem->alloc_small)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_PERMANENT, bufferSize * sizeof(JOCTET)));
				sourceManager->sourceBuffer =(unsigned char*)tempBuffer2;
				sourceManager->sourceBufferSize = bufferSize;
				sourceManager->man.init_source = SourceInit;
				sourceManager->man.fill_input_buffer = SourceFill;
				sourceManager->man.skip_input_data = SourceSkip;
				sourceManager->man.resync_to_restart = jpeg_resync_to_restart;
				sourceManager->man.term_source = SourceTerminate;
				sourceManager->man.bytes_in_buffer = 0;
				sourceManager->man.next_input_byte = NULL;
				jpeg_read_header(&Info, TRUE);
				jpeg_start_decompress(&Info);

				if((Info.output_components != 3) && (Info.output_components != 4))
				{
					if(tempBuffer2!=NULL)
						delete []tempBuffer2;
					if(realFileName)
					{
						if(Buffer!=NULL)
							delete []Buffer;
						Buffer=NULL;
						fclose(fp);
					}
				}

				ImageInfo->imageData=new BYTE[Info.output_width * Info.output_height * 4];

				Stride = Info.output_width * Info.output_components;
				Offset = 0;
				pointer = (*Info.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_IMAGE, Stride, 1);
				while(Info.output_scanline < Info.output_height)
				{
					jpeg_read_scanlines(&Info, pointer, 1);
					memcpy(&ImageInfo->imageData[Offset], pointer[0], Stride);
					Offset += Stride;
				}

				jpeg_finish_decompress(&Info);

				(*reinterpret_cast<unsigned char*>(&Opaque)) = 255;

				if(Info.output_components == 3)
				{
					for(i = (Info.output_width * Info.output_height - 1); i >= 0; i--)
					{
						ImageInfo->imageData[(i * 4) + 3] = Opaque;
						ImageInfo->imageData[(i * 4) + 2] = ImageInfo->imageData[(i * 3) + 2];
						ImageInfo->imageData[(i * 4) + 1] = ImageInfo->imageData[(i * 3) + 1];
						ImageInfo->imageData[(i * 4) + 0] = ImageInfo->imageData[(i * 3) + 0];
					}
				}
				ImageInfo->width = Info.output_width;
				ImageInfo->height = Info.output_height;
				ImageInfo->bpp = 32;
				ImageInfo->type = 0xFFFFFFFF;

				jpeg_destroy_decompress(&Info);
				if(tempBuffer2!=NULL)
					delete []tempBuffer2;
				if(realFileName)
				{
					if(Buffer!=NULL)
						delete []Buffer;
					Buffer=NULL;
					fclose(fp);
				}
				return true;;
			}
		case 1:
			{
				int bufferSize;
				int i;
				int PALETTE_SIZE = 256;
				BLP_RGBA Palette[256];
				BLP_RGBA* TargetPixel;

				memcpy(reinterpret_cast<char*>(Palette), &Buffer[sizeof(BLP_HEADER)], (PALETTE_SIZE * 4));

				bufferSize = header.Width * header.Height;
				ImageInfo->imageData=new BYTE[bufferSize*4];

				switch(header.PictureType)
				{
				case 3:
				case 4:
					{
						BLP_PIXEL* SourcePixel;

						SourcePixel = reinterpret_cast<BLP_PIXEL*>(&Buffer[sizeof(BLP_HEADER) + (PALETTE_SIZE * 4)]);
						TargetPixel = reinterpret_cast<BLP_RGBA*>(ImageInfo->imageData);

						for(i = 0; i < bufferSize; i++)
						{
							TargetPixel[i].Red = Palette[SourcePixel[i].Index].Red;
							TargetPixel[i].Green = Palette[SourcePixel[i].Index].Green;
							TargetPixel[i].Blue = Palette[SourcePixel[i].Index].Blue;
						}

						for(i = 0; i < bufferSize; i++)
						{
							TargetPixel[i].Alpha = SourcePixel[bufferSize + i].Index;
						}
						ImageInfo->width = header.Width;
						ImageInfo->height = header.Height;
						ImageInfo->bpp = 32;
						ImageInfo->type = 0xFFFFFFFF;

						if(realFileName)
						{
							if(Buffer!=NULL)
								delete []Buffer;
							Buffer=NULL;
							fclose(fp);
						}
						return true;
					}

				case 5:
					{
						BLP_PIXEL* SourcePixel;

						SourcePixel = reinterpret_cast<BLP_PIXEL*>(&Buffer[sizeof(BLP_HEADER) + (PALETTE_SIZE * 4)]);
						TargetPixel = reinterpret_cast<BLP_RGBA*>(ImageInfo->imageData);

						for(i = 0; i < bufferSize; i++)
						{
							TargetPixel[i].Red = Palette[SourcePixel[i].Index].Red;
							TargetPixel[i].Green = Palette[SourcePixel[i].Index].Green;
							TargetPixel[i].Blue = Palette[SourcePixel[i].Index].Blue;
							TargetPixel[i].Alpha = 255 - Palette[SourcePixel[i].Index].Alpha;
						}
						ImageInfo->width = header.Width;
						ImageInfo->height = header.Height;
						ImageInfo->bpp = 32;
						ImageInfo->type = 0xFFFFFFFF;

						if(realFileName)
						{
							if(Buffer!=NULL)
								delete []Buffer;
							Buffer=NULL;
							fclose(fp);
						}
						return true;
					}
				default:
					{
						if(realFileName)
							fclose(fp);
						return false;
					}
				}
			}
		default:
			{
				if(realFileName)
					fclose(fp);
				return false;
			}
		}
	}
}