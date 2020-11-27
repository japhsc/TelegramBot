import cv2
import io

def capture_image():
    image = None
    cap = cv2.VideoCapture(0)
    if cap.isOpened():
        rval, frame = cap.read()
        if (rval):
            retval, buffer = cv2.imencode('.jpg', frame)
            if (retval):
                image = io.BytesIO(buffer)
    cap.release()
    return image
    
def capture_video(filename='./test.mp4', duration=10.):
    image = None
    cap = cv2.VideoCapture(0)
    if cap.isOpened():
        fps = cap.get(cv2.CAP_PROP_FPS)
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        frameSize = (width, height)
        frameFormat = cv2.VideoWriter_fourcc(*'mp4v')
        
        num = int(duration*fps)

        #print(num, fps, width, height)
        
        out = cv2.VideoWriter(filename=filename, 
                                fourcc=frameFormat, 
                                fps=fps, 
                                frameSize=frameSize)
        i = 0
        while i<num:
            ret, frame = cap.read()
            if not ret:
                break
            out.write(frame)
            i += 1
        out.release()
        image = open(filename, 'rb')
    cap.release()
    return image
    
import sounddevice as sd
from scipy.io import wavfile
    
def capture_audio(duration=10., fs = 44100):
    data = sd.rec(int(duration*fs), samplerate=fs, channels=1)
    sd.wait()  # Wait until recording is finished
    image = io.BytesIO()
    wavfile.write(image, fs, data)
    return image
    
if  __name__ == '__main__':
    #image = capture_video(duration=2.)
    image = capture_audio(duration=2.)
    print(image)
    '''
    import inspect
    for insp in inspect.getmembers(out):
        print(insp)
    '''
    
