range = 22:27;
dir = '/Users/janmachacek/Muvr/muvr-preclassification/training-data/jan-back-22-05/';
for i = range
    fileName = strcat(strcat(dir, num2str(i)), '.csv');
    M = readtable(fileName);
    ads = table2array(M(:,[5,6,7]));

    % Simple smoothing
    windowSize = 50;
    b = (1 / windowSize) * ones(1, windowSize);
    a = 1;
    adsf = filter(b, a, ads);
    adsf = adsf(windowSize:end,:);

    titleCell = M{1,1:2};
    figure('name', strcat(strcat(titleCell{1}, ' '), titleCell{2}));
    time = 1:length(adsf);
    plot(time, adsf);
end
