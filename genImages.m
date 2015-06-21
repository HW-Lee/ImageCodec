clear all; close all;

ROOTS = {'data', 'results'};

DIRS = {...,
    '1_1536x1024', ...
    '2_1024x768', ...
    '3_1000x728', ...
    '4_1000x1504' ...
};

SIZES = [...
    1536, 1024;
    1024, 768;
    1000, 728;
    1000, 1504;
];
    

NAMES = 'decompressed';
YUVPOSTFIX = '.yuv';
PNGPOSTFIX = '.png';
Nfiles = 3;

for dirIdx = 1:length(DIRS)
    for rootIdx = 1:length(ROOTS)
        for fIdx = 1:Nfiles
            if rootIdx == 2, name = [fullfile(ROOTS{rootIdx} ,DIRS{dirIdx}, NAMES) num2str(fIdx)]; outName = name;
            elseif fIdx == 1, name = fullfile(ROOTS{rootIdx}, DIRS{dirIdx}); outName = fullfile(ROOTS{2} ,DIRS{dirIdx}, 'original');
            else continue;
            end
            rgb = yuv2rgb([name YUVPOSTFIX], SIZES(dirIdx, :));
            imwrite(rgb, [outName PNGPOSTFIX]);
        end
    end
end
clear dirIdx rootIdx fIdx rgb name;
