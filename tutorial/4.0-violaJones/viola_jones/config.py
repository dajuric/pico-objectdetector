# --- stage params
max_trees = 64 #max tree count per stage
maxFPR = 0.5 #max FPR for a stage

# --- cascade params
targetFPR = 1E-3 #target FPR for cascade
minTPRs = [0.980, 0.990, *[0.995] * 2, *[0.997] * 100] #min TPR to retain for each stage
wh_ratio = 1.0 #width to height ratio for object bounding box

# ---- detection params
min_size = 50 #minimal object height to detect
scale_increase = 1.2 #scaling factor to use when for the next image scan (when larger objcts are detected)
step_scale = 0.1 #scaling factor that multiplies a BB size and the resulting value is used to shift BB row-wise and column-wise 