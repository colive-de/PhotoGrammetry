%% ================== 参数设置 ==================
% 数据路径（你可根据需要修改）
dataDir = 'E:\Photogrammetry\Match\Match\images\GTF';

% 结果输出路径
outputDir = 'E:\Photogrammetry\Match\Match\images';
outputFile = fullfile(outputDir, 'res.txt');

% 内点判定阈值（可调）
inlierThreshold = 3.0;   % Sampson Distance < 3 像素


%% ================== 初始化 ==================
letters = 'A':'P';   % A 到 P 共 16 组
results = cell(length(letters), 3);


%% ================== 主循环 ==================
for i = 1:length(letters)

    letter = letters(i);

    % --- 构造文件名 ---
    F_file = fullfile(dataDir, sprintf('%s.txt', letter));
    P_file = fullfile(dataDir, sprintf('%s%s.txt', letter, letter));

    fprintf('Processing %s ...\n', letter);

    % --- 读取真实 F 矩阵 ---
    F = load(F_file);

    % --- 读取匹配点，跳过前 7 行 ---
    raw = importdata(P_file, ' ', 7);
    pts = raw.data;   % 从第8行开始才是匹配点

    % pts 格式： [x1 y1 x2 y2]
    x1 = pts(:,1);
    y1 = pts(:,2);
    x2 = pts(:,3);
    y2 = pts(:,4);

    % --- 组装齐次点坐标 ---
    p1 = [x1 y1 ones(size(x1))]';
    p2 = [x2 y2 ones(size(x2))]';

    % --- 计算 Sampson Distance ---
    % sd = (x2' * F * x1)^2 / (Fx1(1)^2 + Fx1(2)^2 + F'x2(1)^2 + F'x2(2)^2)
    Fx1 = F * p1;
    Ftx2 = F' * p2;
    numer = sum(p2 .* (F * p1), 1).^2;
    denom = Fx1(1,:).^2 + Fx1(2,:).^2 + Ftx2(1,:).^2 + Ftx2(2,:).^2;
    sampsonDist = sqrt(numer ./ denom)';

    % --- RMSE 计算 ---
    RMSE = sqrt(mean(sampsonDist.^2));

    % --- 内点比率计算 ---
    inlierRatio = sum(sampsonDist < inlierThreshold) / length(sampsonDist);

    % 保存结果
    results{i,1} = letter;
    results{i,2} = RMSE;
    results{i,3} = inlierRatio;
end


%% ================== 写入 res.txt ==================
fid = fopen(outputFile, 'w');

for i = 1:size(results,1)
    fprintf(fid, '%s %.6f %.6f\n', results{i,1}, results{i,2}, results{i,3});
end

fclose(fid);

fprintf('\n=== 完成！结果已输出到: %s ===\n', outputFile);


